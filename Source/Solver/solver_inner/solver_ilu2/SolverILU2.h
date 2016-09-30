#ifndef INMOST_SOLVERILU2_H
#define INMOST_SOLVERILU2_H

#include <inmost.h>
#include "solver_ilu2.hpp"
#include "../solver_bcgsl.hpp"

namespace INMOST {

	class SolverILU2 : public SolverInterface {
	private:
		Sparse::Matrix *matrix;
		BCGS_solver *solver;
		Solver::OrderInfo info;

		INMOST_DATA_ENUM_TYPE additive_schwartz_overlap;

        INMOST_DATA_ENUM_TYPE maximum_iterations;
        INMOST_DATA_REAL_TYPE absolute_tolerance;
        INMOST_DATA_REAL_TYPE relative_tolerance;
        INMOST_DATA_REAL_TYPE divergence_tolerance;

        INMOST_DATA_REAL_TYPE preconditioner_drop_tolerance;
        INMOST_DATA_REAL_TYPE preconditioner_reuse_tolerance;
        INMOST_DATA_REAL_TYPE preconditioner_fill_level;
        INMOST_DATA_ENUM_TYPE preconditioner_rescale_iterations;
        INMOST_DATA_ENUM_TYPE solver_gmres_substeps;
	public:
        SolverILU2();
        SolverILU2(const SolverInterface* other);

		virtual void Assign(const SolverInterface* other);

        virtual void Initialize(int *argc, char ***argv, const char *parameters_file, std::string prefix);
		virtual void SetMatrix(Sparse::Matrix & A, bool ModifiedPattern, bool OldPreconditioner);
        virtual bool Solve(Sparse::Vector &RHS, Sparse::Vector &SOL);
        virtual bool Clear();

    	virtual bool isMatrixSet();

		virtual INMOST_DATA_REAL_TYPE GetPropertyReal(std::string property) const;
		virtual INMOST_DATA_ENUM_TYPE GetPropertyEnum(std::string property) const;

		virtual void SetPropertyReal(std::string property, INMOST_DATA_REAL_TYPE value);
        virtual void SetPropertyEnum(std::string property, INMOST_DATA_ENUM_TYPE value);

		virtual const INMOST_DATA_ENUM_TYPE Iterations() const;
		virtual const INMOST_DATA_REAL_TYPE Residual() const;
		virtual const std::string ReturnReason() const;

		virtual const std::string SolverName() const;

		virtual void Finalize();
        virtual ~SolverILU2();
	};

}

#endif