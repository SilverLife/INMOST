#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdio>

#include "inmost.h"

using namespace INMOST;

#if defined(USE_MPI)
#define BARRIER MPI_Barrier(MPI_COMM_WORLD);
#else
#define BARRIER
#endif

// developed by Igor Konshin
#define NMAX 1024 //1024 // maximal pixel field dimension

static int f[NMAX][NMAX]; // pixel field
static int nm; // actual matrix dimention
static int nf; // actual field dimention
static double sc; // scaling indices from matrix to pixel field

// Initialize print of matrix portrait into PS file
void ps_init (int n)
{
    nm = n;
    nf = (n <= NMAX) ? n : NMAX;
    sc = 1;
    if (nm > 1 && nm > nf) sc = (double)(nf-1)/(double)(nm-1);
    std::cout<<"::: nm="<<nm<<" nf="<<nf<<" sc="<<sc<<std::endl;//db!
    for (int i=0; i<nf; i++)
        for (int j=0; j<nf; j++)
            f[i][j] = 0;
}

// Save 1 nonzero into some pixel (j and j are started from 0 like in C)
inline void ps_ij (int i, int j)
{
    if (i >= 0 && i < nm && j >= 0 && j < nm)
        f[(int)(j*sc)][nf-1-(int)(i*sc)] = 1;
}

// Write the collected matrix portrait into PS file
void ps_file (std::string file, int nbl = 1, int * ibl = NULL)
{
    double s1 = 50.0;
    double s = 500.0 / nf;

    std::fstream output(file.c_str(),std::ios::out);
    output << "%%BoundingBox: 0 0 600 600" << std::endl;
    output << "/m {moveto} def % x y" << std::endl;
    output << "/l {lineto} def % x y" << std::endl;
    output << "/s {stroke} def % x y" << std::endl;
    output << "/n {newpath} def % x y" << std::endl;
    output << "/c {closepath} def % x y" << std::endl;
    output << 0.1 << " setlinewidth" << std::endl;
    output << "n " << s1+s*.5 << " " << s1+s*.5 << " m "
           << s1+s*(nf+.5) << " " << s1+s*.5 << " l "
           << s1+s*(nf+.5) << " " << s1+s*(nf+.5) << " l "
           << s1+s*.5 << " " << s1+s*(nf+.5) << " l "
           << s1+s*.5 << " " << s1+s*.5 << " l s c" << std::endl;
    if (nbl > 1) {
        for (int i=1; i<nbl; i++) {
            double x = sc*ibl[i] + .5;
            double y = nf - (sc*ibl[i] + .5) + 1.0;
            output << "n " << s1+s*.5 << " " << s1+s*y << " m "
                   << s1+s*(nf+.5) << " " << s1+s*y << " l s c" << std::endl;
            output << "n " << s1+s*x << " " << s1+s*.5 << " m "
                   << s1+s*x << " " << s1+s*(nf+.5) << " l s c" << std::endl;
        }
    }
    double r = (log10(1.0*nf) + 1.0) / 4.0;
    r = s * ((r < 1.0) ? r : 1.0) / 2.0;
    r = (r > 1.0) ? r : 1.0;
    //r = .2;
    output << 2*r << " setlinewidth" << std::endl;
    output << "/d {moveto currentpoint" << r << " 0 360 arc fill} def % x y" << std::endl;
    output << "n" << std::endl;

    std::stringstream ps(std::ios::in | std::ios::out);
    //ps << std::scientific;
    //ps.precision(2);

    for (int i=0; i<nf; i++)
        for (int j=0; j<nf; j++)
            if (f[i][j] != 0)
                ps << s1+s*(i+1)-r << " " << s1+s*(j+1) << " m "
                   << s1+s*(i+1)+r << " " << s1+s*(j+1) << " l" << std::endl;

    output << ps.rdbuf();
    output << "s c" << std::endl;
    output << "showpage" << std::endl;
}

// Print of matrix portrait into PS file
void ps_crs (int n, int nbl, int * ibl, int * ia, int *ja, const std::string file)
{
    int ia0 = ia[0];
    int ja0 = n;
    for (int k=0; k<ia[nbl]-ia0; k++) ja0 = (ja0 < ja[k]) ? ja0 : ja[k];
    ps_init (n);
    for (int i=0; i<n; i++)
        for (int k=ia[i]-ia0; k<ia[i+1]-ia0; k++)
            ps_ij (i, ja[k]-ja0);
    ps_file (file, nbl, ibl);
}

#define USE_INMOST
#if defined(USE_INMOST)
// Print of INMOST matrix portrait into PS file
void ps_inmost (Sparse::Matrix & A, const std::string file)
{
    int n = A.Size();
    int row = A.GetFirstIndex();
    ps_init (n);
    for (Sparse::Matrix::iterator it = A.Begin(); it != A.End(); ++it) {
        for (Sparse::Row::iterator jt = it->Begin(); jt != it->End(); ++jt)
            ps_ij (row,jt->first);
        row++;
    }
    ps_file (file, NULL, NULL);
}
#endif

//#define PS_MATR_SELFTEST_CRS
#if defined (PS_MATR_SELFTEST_CRS)
int main ()
{
    //       [  3 -1 -1  0 ]
    //   A = [ -1  3  0 -1 ]
    //       [ -1  0  3 -1 ]
    //       [  0 -1 -1  3 ]
    static int n=4; // nza=12;
    int   ia[5]  = { 0, 3, 6, 9, 12 }; //ia[n+1]
    int   ja[12] = { 1,2,3, 1,2,4, 1,3,4, 2,3,4 }; //ja[nza]
    int nbl=2, ibl[3]={0,2,4};
    ps_crs (n,nbl,ibl,ia,ja,"z.ps");
}
#endif

//#define PS_MATR_SELFTEST_0
#if defined (PS_MATR_SELFTEST_0)
int main ()
{
    int nbl=3, ibl[4]={0,30,60,100};
    ps_init (100);
    ps_ij (0,0);
    ps_ij (1,1);
    ps_ij (2,2);
    ps_ij (12,17);
    ps_ij (79,89);
    ps_ij (99,99);
    ps_file ("z.ps",nbl,ibl);
}
#endif

int main(int argc, char **argv) {
    int processRank = 0, processorsCount = 1;

#if defined(USE_MPI)
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &processRank);  // Get the rank of the current process
    MPI_Comm_size(MPI_COMM_WORLD, &processorsCount); // Get the total number of processors used
#endif

    {
        std::string matrixFileName = "";
        std::string vectorBFileName = "";
        std::string vectorXFileName = "";
        std::string parametersFileName = "";
        std::string solverName = "inner_ilu2";
        std::string orderingFileName = "";


        bool matrixFound = false;
        bool vectorBFound = false;
        bool vectorXFound = false;
        bool parametersFound = false;
        bool typeFound = false;
        bool saveVector = false;
        bool orderingFound = false;

        //Parse argv parameters
        if (argc == 1) goto helpMessage;
        int i;
        for (i = 1; i < argc; i++) {
            //Print help message and exit
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                helpMessage:
                if (processRank == 0) {
                    std::cout << "Help message: " << std::endl;
                    std::cout << "Command line options: " << std::endl;
                    std::cout << "Required: " << std::endl;
                    std::cout << "-m, --matrix <Matrix file name>" << std::endl;
                    std::cout << "Optional: " << std::endl;
                    std::cout << "-b, --bvector <RHS vector file name>" << std::endl;
                    std::cout << "-x, --xvector <X vector file name>" << std::endl;
                    std::cout << "-d, --database <Solver parameters file name>" << std::endl;
                    std::cout << "-t, --type <Solver type name>" << std::endl;
                    std::cout << "-ord, --ordering <Ordering file name>" << std::endl;
                    std::cout << "-s, --save" << std::endl;
                    std::cout << "  Available solvers:" << std::endl;
                    Solver::Initialize(NULL, NULL, NULL);
                    std::vector<std::string> availableSolvers = Solver::getAvailableSolvers();
                    for (solvers_names_iterator_t it = availableSolvers.begin(); it != availableSolvers.end(); it++) {
                        std::cout << "      " << *it << std::endl;
                    }
                    Solver::Finalize();
                }
                return 0;
            }
            //Matrix file name found with -m or --matrix options
            if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--matrix") == 0) {
                matrixFound = true;
                matrixFileName = std::string(argv[i + 1]);
                FILE *matrixFile = fopen(matrixFileName.c_str(), "r");
                if (matrixFile == NULL) {
                    if (processRank == 0) {
                        std::cout << "Matrix file not found: " << argv[i + 1] << std::endl;
                        exit(1);
                    }
                } else {
                    if (processRank == 0) {
                        std::cout << "Matrix file found: " << argv[i + 1] << std::endl;
                    }
                }
                fclose(matrixFile);
                i++;
                continue;
            }
            //B vector file name found with -b or --bvector options
            if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--bvector") == 0) {
                if (processRank == 0) {
                    std::cout << "B vector file found: " << argv[i + 1] << std::endl;
                }
                vectorBFound = true;
                vectorBFileName = std::string(argv[i + 1]);
                i++;
                continue;
            }
            //X vector file name found with -x or --xvector options
            if (strcmp(argv[i], "-x") == 0 || strcmp(argv[i], "--xvector") == 0) {
                if (processRank == 0) {
                    std::cout << "X vector file found: " << argv[i + 1] << std::endl;
                }
                vectorXFound = true;
                vectorXFileName = std::string(argv[i + 1]);
                i++;
                continue;
            }
            //Parameters file name found with -d or --database options
            if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--database") == 0) {
                if (processRank == 0) {
                    std::cout << "Solver parameters file found: " << argv[i + 1] << std::endl;
                }
                parametersFound = true;
                parametersFileName = std::string(argv[i + 1]);
                i++;
                continue;
            }
            //Ordering file name found with -ord or --parameters options
            if (strcmp(argv[i], "-ord") == 0 || strcmp(argv[i], "--ordering") == 0) {
                if (processRank == 0) {
                    std::cout << "Ordering file found: " << argv[i + 1] << std::endl;
                }
                orderingFound = true;
                orderingFileName = std::string(argv[i + 1]);
                i++;
                continue;
            }
            if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--save") == 0) {
                saveVector = true;
                continue;
            }
            //Solver type found with -t ot --type options
            if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--type") == 0) {
                if (processRank == 0) {
                    std::cout << "Solver type index found: " << argv[i + 1] << std::endl;
                }
                typeFound = true;
                solverName = std::string(argv[i + 1]);
                i++;
                continue;
            }
        }

        if (!matrixFound) {
            if (processRank == 0) {
                std::cout <<
                          "Matrix not found, you can specify matrix file name using -m or --matrix options, otherwise specify -h option to see all options, exiting...";
            }
            return -1;
        }

        if (!typeFound) {
            if (processRank == 0) {
                std::cout <<
                          "Solver type not found in command line, you can specify solver type with -t or --type option, using INNER_ILU2 solver by default."
                          <<
                          std::endl;
            }
        }

        if (!vectorBFound) {
            if (processRank == 0) {
                std::cout <<
                          "B vector not found, you can specify b vector file name with -b or --bvector option, using identity vector by default."
                          <<
                          std::endl;
            }
        }

        // Initialize the linear solver in accordance with args
        Solver::Initialize(&argc, &argv, parametersFound ? parametersFileName.c_str() : NULL);

        if (!Solver::isSolverAvailable(solverName)) {
            if (processRank == 0) {
                std::cout << "Solver " << solverName << " is not available" << std::endl;
            }
            Solver::Finalize();
            exit(1);
        }

        Solver solver = Solver(solverName, "test");

        if (processRank == 0) {
            std::cout << "Solving with " << solverName << std::endl;
        }

        Sparse::Matrix mat("A"); // Declare the matrix of the linear system to be solved
        Sparse::Vector b("rhs"); // Declare the right-hand side vector
        Sparse::Vector x("sol"); // Declare the solution vector
        double tempTimer = Timer(), solvingTimer;
        if (orderingFound) {
            if (processRank == 0) {
                std::cout << "Using ordering file " << orderingFileName << std::endl;
            }
            mat.Load(matrixFileName, ENUMUNDEF, ENUMUNDEF, orderingFileName);
        } else {
            mat.Load(matrixFileName); //if interval parameters not set, matrix will be divided automatically
        }
        if (processorsCount == 1) {
            ps_inmost(mat, "a.ps"); //db !IK!
        }
        BARRIER
        if (processRank == 0) {
            std::cout << "load matrix:    " << Timer() - tempTimer << std::endl;
        }
        tempTimer = Timer();
        if (vectorBFound) {
            if (orderingFound) {
                b.Load(vectorBFileName, ENUMUNDEF, ENUMUNDEF, orderingFileName); // Load RHS vector with ordering
            } else {
                b.Load(vectorBFileName); //if interval parameters not set, matrix will be divided automatically
            }
            //b.Save(vectorBFileName + "_saved_test.rhs");
        } else { // Set local RHS to 1 if it was not specified
            INMOST_DATA_ENUM_TYPE mbeg, mend, k;
            mat.GetInterval(mbeg, mend);
            b.SetInterval(mbeg, mend);
            for (k = mbeg; k < mend; ++k) b[k] = 1.0;
        }
        BARRIER
        if (processRank == 0) {
            std::cout << "load vector:    " << Timer() - tempTimer << std::endl;
        }
        solvingTimer = Timer();
        int iters;
        bool success;
        double resid, realresid = 0;
        std::string reason;
        BARRIER

        tempTimer = Timer();
        solver.SetMatrix(mat);
        //s.SetMatrix(mat); // Compute the preconditioner for the original matrix
        BARRIER
        if (processRank == 0) std::cout << "preconditioner time: " << Timer() - tempTimer << std::endl;
        tempTimer = Timer();
        success = solver.Solve(b, x); // Solve the linear system with the previously computted preconditioner
        BARRIER
        solvingTimer = Timer() - solvingTimer;
        if (processRank == 0) std::cout << "iterations time:     " << Timer() - tempTimer << std::endl;
        iters = solver.Iterations(); // Get the number of iterations performed
        resid = solver.Residual();   // Get the final residual achieved
        reason = solver.ReturnReason(); // Get the convergence reason

        if (saveVector) {
            x.Save("output.sol");  // Save the solution if required
        }

        // Compute the true residual
        double aresid = 0, bresid = 0;
        Sparse::Vector test;
        tempTimer = Timer();
        Solver::OrderInfo info;
        info.PrepareMatrix(mat, 0);
        info.PrepareVector(x);
        info.Update(x);

        mat.MatVec(1.0, x, 0.0, test); // Multiply the original matrix by a vector
        {
            INMOST_DATA_ENUM_TYPE mbeg, mend, k;
            info.GetLocalRegion(info.GetRank(), mbeg, mend);
            for (k = mbeg; k < mend; ++k) {
                aresid += (test[k] - b[k]) * (test[k] - b[k]);
                bresid += b[k] * b[k];
            }
        }
        double temp[2] = {aresid, bresid}, recv[2] = {aresid, bresid};
#if defined(USE_MPI)
        MPI_Reduce(temp, recv, 2, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
#endif
        realresid = sqrt(recv[0] / (recv[1] + 1.0e-100));
        info.RestoreVector(x);
        if (processRank == 0) {
            std::cout << "||Ax-b||=" << sqrt(recv[0]) << " ||b||=" << sqrt(recv[1]) << " ||Ax-b||/||b||=" <<
                      sqrt(recv[0] / (recv[1] + 1.0e-100)) << std::endl;
            std::cout << "norms: " << Timer() - tempTimer << std::endl;
            std::cout << processorsCount << " processors for Solver " << solverName;
            if (success) {
                std::cout << " solved in " << solvingTimer << " secs";
                std::cout << " with " << iters << " iterations to " << resid << " norm";
            } else std::cout << " failed to solve with " << iters << "iterations";
            std::cout << " matrix \"" << matrixFileName << "\"";
            if (vectorBFound) std::cout << " vector \"" << vectorBFileName << "\"";
            std::cout << " true residual ||Ax-b||/||b||=" << realresid;
            std::cout << std::endl;
            std::cout << "reason: " << reason << std::endl;
        }

        if (vectorXFound) {
            Sparse::Vector ex("exact");  // Declare the exact solution vector
            Sparse::Vector err("error"); // Declare the solution error vector
            INMOST_DATA_ENUM_TYPE mbeg, mend, k;
            mat.GetInterval(mbeg, mend);
            err.SetInterval(mbeg, mend);
            ex.Load(vectorXFileName);
            BARRIER
            double dif1 = 0, dif2 = 0, dif8 = 0, norm = 0;
            for (k = mbeg; k < mend; ++k) {
                double dloc = err[k] = fabs(x[k] - ex[k]);
                dif1 += dloc;
                dif2 += dloc * dloc;
                dif8 = (dif8 > dloc) ? dif8 : dloc;
                norm += fabs(ex[k]);
            }
#if defined(USE_MPI)
            if (processorsCount > 1) {
                double temp1 = dif1;
                MPI_Reduce(&temp1, &dif1, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
                temp1 = dif2;
                MPI_Reduce(&temp1, &dif2, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
                temp1 = dif8;
                MPI_Reduce(&temp1, &dif8, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
                temp1 = norm;
                MPI_Reduce(&temp1, &norm, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
            }
#endif
            dif2 = sqrt(dif2);
            norm += 1.0e-100;
            if (processRank == 0) {
                std::cout << "Difference with exact solution \"" << vectorXFileName << "\": " << std::scientific <<
                          std::setprecision(6) << std::endl;
                std::cout << "dif1 = " << dif1 << "  dif2 = " << dif2 << "  dif8 = " << dif8 << "  ||ex||_1 = " <<
                          norm << std::endl;
                std::cout << "rel1 = " << dif1 / norm << "  rel2 = " << dif2 / norm << "  rel8 = " << dif8 / norm <<
                          std::endl;
            }
        }
    }
    BARRIER
    Solver::Finalize(); // Finalize solver and close MPI activity
    return 0;
}
