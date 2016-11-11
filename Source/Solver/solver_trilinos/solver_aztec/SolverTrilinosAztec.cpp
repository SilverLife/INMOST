#include "SolverTrilinosAztec.h"

namespace INMOST {

    SolverTrilinosAztec::SolverTrilinosAztec(SolverParameters &parameters): SolverTrilinos(parameters) {

    }

    SolverTrilinosAztec::SolverTrilinosAztec(const SolverInterface *other): SolverTrilinos(other) {
        //You should not really want to copy solver's information
        throw INMOST::SolverUnsupportedOperation;
    }

    bool SolverTrilinosAztec::Solve(INMOST::Sparse::Vector &RHS, INMOST::Sparse::Vector &SOL) {
        std::string name = Epetra_problem->first;
        Epetra_LinearProblem *Epetra_linear_problem = &Epetra_problem->second;
        Epetra_Vector VectorRHS(View, matrix->Map(), &*RHS.Begin());
        Epetra_Vector VectorSOL(View, matrix->Map(), &*SOL.Begin());
        Epetra_linear_problem->SetRHS(&VectorRHS);
        Epetra_linear_problem->SetLHS(&VectorSOL);

        bool have_params = parameters_file != "";
        const Teuchos::RCP<Teuchos::ParameterList> top_level_params = Teuchos::createParameterList();
        Teuchos::ParameterList local_list;
        if (have_params) {
            Teuchos::updateParametersFromXmlFileAndBroadcast(parameters_file, top_level_params.ptr(),
                                                             Teuchos::MpiComm<int>(Teuchos::opaqueWrapper(
                                                                     RHS.GetCommunicator())));
            if (!top_level_params->isSublist(name))
                have_params = false;
            else {
                local_list = top_level_params->sublist(name);
            }
        }

        AztecOO AztecSolver(*Epetra_linear_problem);

        if (have_params && local_list.isSublist("AztecOO")) {
            Teuchos::ParameterList AztecOOParams = local_list.sublist("AztecOO");
            if (AztecOOParams.isParameter("Max Iterations")) {
                parameters.set("maximum_iterations", to_string(AztecOOParams.get<int>("Max Iterations")));
            }
            if (AztecOOParams.isParameter("Tolerance")) {
                parameters.set("relative_tolerance", to_string(AztecOOParams.get<double>("Tolerance")));
            }
            if (AztecOOParams.isSublist("AztecOO Settings")) {
                AztecSolver.SetParameters(AztecOOParams.sublist("AztecOO Settings"));
            }
        } else {
            AztecSolver.SetAztecOption(AZ_diagnostics, AZ_none);
            AztecSolver.SetAztecOption(AZ_output, AZ_none);
            AztecSolver.SetAztecOption(AZ_solver, AZ_bicgstab);
            AztecSolver.SetAztecOption(AZ_overlap, parameters.get<INMOST_DATA_ENUM_TYPE>("additive_schwartz_overlap"));
        }

        if (!have_params) {
            AztecSolver.SetAztecParam(AZ_drop, parameters.get<INMOST_DATA_REAL_TYPE>("drop_tolerance"));
            AztecSolver.SetAztecParam(AZ_ilut_fill, parameters.get<INMOST_DATA_REAL_TYPE>("fill_level"));
        }

        AztecSolver.Iterate(parameters.get<INMOST_DATA_ENUM_TYPE>("maximum_iterations"), parameters.get<INMOST_DATA_ENUM_TYPE>("relative_tolerance"));
        const double *stats = AztecSolver.GetAztecStatus();
        bool success = true;
        std::string reason = "";
        TrilinosCheckStatus(static_cast<int>(stats[AZ_why]), success, reason);
        lastIterations = static_cast<INMOST_DATA_ENUM_TYPE>(AztecSolver.NumIters());
        lastResidual = AztecSolver.TrueResidual();
        returnReason = reason;
        return success;
    }

    const std::string SolverTrilinosAztec::SolverName() const {
        return "trilinos_aztec";
    }

    SolverTrilinosAztec::~SolverTrilinosAztec() {
        this->Clear();
    }

}