/**
 * Copyright (c) 2023, James Nutaro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies, 
 * either expressed or implied, of the FreeBSD Project.
 *
 * Bugs, comments, and questions can be sent to nutaro@gmail.com
 */
#ifndef _adevs_trap_h_
#define _adevs_trap_h_
#include <cmath>
#include <cassert>
#include <algorithm>
#include <cstring>
#include <cfloat>
#include "adevs_hybrid.h"
#include <kinsol/kinsol.h>
#include <nvector/nvector_serial.h>
#include <sunmatrix/sunmatrix_dense.h>
#include <sundials/sundials_types.h> 
#include <sunlinsol/sunlinsol_dense.h>

namespace adevs
{

/**
 * This is the second order accurate trapezoidal method. You must supply
 * a jacobian to use this integration method. 
 */
template <typename X> class trap:
	public ode_solver<X>
{
	public:
		/**
		 * Create an integrator that will use the given
		 * maximum step size. The tolerance is used to
		 * terminate the newton iteration used at each
		 * integration step.
		 * @param sys The system to solve
		 * @param err_tol Truncation error limit. 
		 * @param h_max Maximum allowed step size
		 * @param silent If set to true, the KINSOL error and info messages are surpressed.
		 * The default value is false.
		 */
		trap(ode_system<X>* sys, double err_tol, double h_max, bool silent = false);
		/// Destructor
		~trap();
		double integrate(double* q, double h_lim);
		void advance(double* q, double h);
	private:
		double *k; // Fixed term in newton iteration
		double *dq; // Derivatives at guess
		// The Euler initial guess which we also
		// use for step size control
		double *guess;
		const double err_tol;
		const double h_max; // Maximum time step
		double h_cur;

		// Advance the solution q by h. Return result by overwriting q.
		// Returns true on success. On failure, returns false and q is
		// left alone. The dq0 argument contains the derivative f(q).
		bool step(double* q, double h, const double *dq0);

		// Data for KINSOL non linear system solver
		N_Vector y, scale;
		void *kmem;
		SUNMatrix J;
		SUNLinearSolver LS;

		struct kinsol_data_t
		{
			trap<X>* self;
			double h;
		};

		kinsol_data_t kinsol_data;

		void prep_kinsol(bool silent);

		static int func(N_Vector y, N_Vector f, void* user_data);
		static int jac(N_Vector y, N_Vector f, SUNMatrix J,
			void *user_data, N_Vector tmp1, N_Vector tmp2);
		static int check_retval(void *retvalvalue, const char *funcname, int opt);
		static void silent_error_handler(int,const char*,const char*,char*,void*){}
		static void silent_info_handler(const char*,const char*,char*,void*){}
};

template <typename X>
int trap<X>::check_retval(void *retvalvalue, const char *funcname, int opt)
{
	int *errretval;
	/* Check if SUNDIALS function returned NULL pointer - no memory allocated */
	if (opt == 0 && retvalvalue == NULL) {
		return(1);
	}
	/* Check if retval < 0 */
	else if (opt == 1) {
		errretval = (int *) retvalvalue;
		if (*errretval < 0) {
			return(1);
		}
	}
	/* Check if function returned NULL pointer - no memory allocated */
	else if (opt == 2 && retvalvalue == NULL) {
		return(1);
	}
	return(0);
}

template <typename X>
int trap<X>::func(N_Vector y, N_Vector f, void* user_data)
{
	realtype* yd = N_VGetArrayPointer(y);
	realtype* fd = N_VGetArrayPointer(f);
	kinsol_data_t* data = static_cast<kinsol_data_t*>(user_data);
	data->self->sys->der_func(yd,fd);
	for (int i = 0; i < data->self->sys->numVars(); i++)
		fd[i] = data->self->k[i]+fd[i]*(data->h/2.0)-yd[i];
	return 0;
}

template <typename X>
int trap<X>::jac(N_Vector y, N_Vector f, SUNMatrix J,
	void *user_data, N_Vector tmp1, N_Vector tmp2)
{
	realtype* yd = N_VGetArrayPointer(y);
	realtype* Jd = SUNDenseMatrix_Data(J);
	kinsol_data_t* data = static_cast<kinsol_data_t*>(user_data);
	const int N = data->self->sys->numVars();
	data->self->sys->get_jacobian(yd,Jd);
	// Get the matrix for the linear solver
	for (int i = 0; i < N*N; i++)
		Jd[i] *= data->h/2.0;
	for (int i = 0; i < N; i++)
		Jd[i*(N+1)] -= 1.0;
	return 0;
} 

template <typename X>
void trap<X>::prep_kinsol(bool silent)
{
	int retval;
	kinsol_data.self = this;
	/* Create vectors for solution, scales, and jacobian */
	y = N_VNew_Serial(this->sys->numVars());
	if (check_retval((void *)y, "N_VNew_Serial", 0))
		throw adevs::exception("N_VNew_Serial failed");
	scale = N_VClone(y);
	if (check_retval((void *)scale, "N_VClone_Serial", 0))
		throw adevs::exception("N_VClone failed");
	// No scaling
	N_VConst(RCONST(1.0),scale);
	J = SUNDenseMatrix(this->sys->numVars(), this->sys->numVars());
	if(check_retval((void *)J, "SUNDenseMatrix", 0))
		throw adevs::exception("SUNDenseMatrix failed");
	/* Create dense SUNLinearSolver object */
	LS = SUNLinSol_Dense(y, J);
	if(check_retval((void *)LS, "SUNLinSol_Dense", 0))
		throw adevs::exception("SUNLinSol_Dense failed");
	/* Initialize and allocate memory for KINSOL */
	kmem = KINCreate();
	if (check_retval((void *)kmem, "KINCreate", 0))
		throw adevs::exception("KINCreate failed");
	retval = KINInit(kmem, func, y); /* y passed as a template */
	if (check_retval(&retval, "KINInit", 1))
		throw adevs::exception("KINInit failed");
	retval = KINSetUserData(kmem, &kinsol_data);
	if (check_retval(&retval, "KINSetUserData", 1))
		throw adevs::exception("KINSetUserData failed");
	retval = KINSetLinearSolver(kmem, LS, J);
	if (check_retval(&retval, "KINSetScaledStepTol", 1)) 
		throw adevs::exception("KINSetScaledStepTol failed");
	/* If there is support for symbolic jacobian, then register
	   the function. Otherwise KINSOL provides a numerical
	   approximation. */
	if (this->sys->get_jacobian(NULL,NULL))
	{
		retval = KINSetJacFn(kmem, jac);
		if (check_retval(&retval, "KINSetJacFn", 1))
			throw adevs::exception("KINSetJacFn failed");
	}
	if (silent)
	{
		KINSetErrHandlerFn(kmem,silent_error_handler,NULL);
		KINSetInfoHandlerFn(kmem,silent_info_handler,NULL);
	}
}

template <typename X>
trap<X>::trap(ode_system<X>* sys, double err_tol, double h_max, bool silent):
	ode_solver<X>(sys),err_tol(err_tol),h_max(h_max),h_cur(h_max)
{
	guess = new double[sys->numVars()];
	dq = new double[sys->numVars()];
	k = new double[sys->numVars()];

	prep_kinsol(silent);
}

template <typename X>
trap<X>::~trap()
{
	delete [] guess;
	delete [] dq;
	delete [] k;
	N_VDestroy(y);
	N_VDestroy(scale);
	KINFree(&kmem);
	SUNLinSolFree(LS);
	SUNMatDestroy(J);
}

template <typename X>
void trap<X>::advance(double* q, double h)
{
	double dt;
	while ((dt = integrate(q,h)) < h) h -= dt;
}

template <typename X>
bool trap<X>::step(double* q, double h, const double* dq0)
{
	const int N = this->sys->numVars();
	realtype* yd = N_VGetArrayPointer(y);
	// Fixed term in the trapezoidal integration rule
	for (int i = 0; i < N; i++) 
	{
		k[i] = q[i]+(h/2.0)*dq0[i];
		// Use Euler to get the initial guess
		guess[i] = yd[i] = q[i]+h*dq0[i];
	}
	kinsol_data.h = h;
	int retval = KINSol(kmem,           /* KINSol memory block */
		y,              /* initial guess on input; solution vector */
		KIN_LINESEARCH, /* global strategy choice */
		scale,          /* scaling vector, for the variable cc */
		scale);         /* scaling vector for function values fval */
	if (retval < 0)
		return false;
	memcpy(q,yd,sizeof(double)*N);
	return true;
}

template <typename X>
double trap<X>::integrate(double* q, double h_lim)
{
	// Pick the step size based on our guess for the best
	// and the supplied limits
	double h = std::min<double>(h_cur,std::min<double>(h_max,h_lim));
	if (h == 0.0)
		return h;
	// Next step size guess is our maximum allowed
	h_cur = h_max;
	// Get the derivatives at the current point
	this->sys->der_func(q,dq);
	// Shrink the error until we get convergence of the nonlinear solver.
	// Shrink our next step size so that we are likely to get convergence
	// next time around.
	while (!step(q,h,dq)) { h *= 0.8; h_cur = h; }
	// Euler step - Trap step. This is approximately equal to the
	// Euler truncation error C h^2 where C is some constant.
	// Use this to look for a smaller choice of step if we need one.
	// This smaller choice will be smaller than the limit supplied by
	// the user and the limit imposed by convergence of the nonlinear 
	// system solver
	for (int i = 0; i < this->sys->numVars(); i++)
	{	
		double C = fabs(guess[i]-q[i])/(h*h);
		// If the guesses are very close, then don't bother with
		// the error estimate
		if (C < FLT_MIN)
			continue;
		double hh = sqrt(err_tol/C);
		// Next step size.
		h_cur = std::min(h_cur,hh);
	}
	// Return the step size that we used
	return h;
}

} // end of namespace
#endif
