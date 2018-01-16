#include <Rcpp.h>
#include <boost/math/distributions/beta.hpp>
#include <random>
#include <algorithm>
#include <iterator>
using namespace Rcpp;
using namespace boost::math;
using namespace std;

// helper function to create a vector of random variables that follows uniform distribution
void generateUniformRealNumbers(vector<double> &array, const double lower, const double upper){
  random_device rd; 
  mt19937 engine(rd());
  uniform_real_distribution<> rUniform(lower, upper);
  auto gen = bind(rUniform, engine);
  generate(begin(array), end(array), gen);
}

// [[Rcpp::export]]
NumericVector simulateBetaDistribution(NumericVector x, SEXP alpha, SEXP beta, SEXP nSim) {
  auto alpha_ = Rcpp::as<double>(alpha);
  auto beta_ = Rcpp::as<double>(beta);
  long noOfSim = Rcpp::as<int>(nSim);
  
  vector<double> rUniforms(noOfSim);
  generateUniformRealNumbers(rUniforms, 0.0, alpha_);
  
  vector<double> targetUniform(noOfSim);
  generateUniformRealNumbers(targetUniform, 0.0, 1.0);;
  
  beta_distribution<> mybeta(alpha_, beta_);
  vector<double> result;
  for(int i = 0; i < noOfSim; i++){
    if(rUniforms[i] < pdf(mybeta, targetUniform[i])){
      result.push_back(targetUniform[i]);
    }
  }
  
  return wrap(result);
}



/*** R
simulateBetaDistribution(42, 2.7, 6.3, 2500)
*/
