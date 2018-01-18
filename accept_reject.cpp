#include <Rcpp.h>
#include <boost/math/distributions/beta.hpp>
#include <random>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <functional>
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

//get the bound M
double getBoundM(int length, double alpha, double beta) {
  double first{0.0}, last{1.0};
  vector<double> dvec(length + 1);
  iota(begin(dvec), end(dvec), first);
  transform(begin(dvec), end(dvec), begin(dvec), [first, last, length](double i){ return first + (last - first) * i / length;});

  vector<double> result(length + 1);
  beta_distribution<> mybeta(alpha, beta);
  transform(begin(dvec), end(dvec), begin(result), [&mybeta](double item){ return pdf(mybeta, item); });
  return *max_element(begin(result), end(result));
}

// [[Rcpp::export]]
NumericVector simulateBetaDistribution(SEXP alpha, SEXP beta, SEXP nSim) {
  Environment stats("package:stats");
  Function f = stats["optimize"];
  
  auto alpha_ = Rcpp::as<double>(alpha);
  auto beta_ = Rcpp::as<double>(beta);
  auto noOfSim = Rcpp::as<int>(nSim);

  auto boundM = getBoundM(noOfSim, alpha_, beta_);
  
  vector<double> rUniforms(noOfSim);
  generateUniformRealNumbers(rUniforms, 0.0, boundM);
  
  vector<double> yUniform(noOfSim);
  generateUniformRealNumbers(yUniform, 0.0, 1.0);;
  
  beta_distribution<> mybeta(alpha_, beta_);
  vector<double> result;
  for(int i = 0; i < noOfSim; i++){
    if(rUniforms[i] < pdf(mybeta, yUniform[i])){
      result.push_back(yUniform[i]);
    }
  }

  return wrap(result);
}

/*** R
simulateBetaDistribution(2.7, 6.3, 2500)
*/
