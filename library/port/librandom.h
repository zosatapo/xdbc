/**
 * librandom.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef  __OPENCODE_RANDOM_H__
#define  __OPENCODE_RANDOM_H__

#include "port/libdefs.h"
#include "port/libnumber.h"

__OPENCODE_BEGIN_NAMESPACE

const int vnl_random_array_size = 37;

/** 
 * A superior random number generator.
 * Implements a new random number generator that
 * recently appeared in the literature. It generates 32 bit
 * numbers with a higher degree of randomness than previous
 * generators and has a cycle of 10^354 i.e. so huge that in
 * practice it never cycles.
 * For the mathematics behind it see:
 * "A New Class of Random Number Generators" G. Marsaglia and A. Zaman,
 * Annals of Applied Probability 1991, Vol. 1, No. 3, 462.
 */
class __OPENCODE_CORE_API_DECL Random
{
public:
  /** Destructor */
  ~Random();

  /**
   * Default constructor.
   * Initializes the random number generator non-deterministically.
   * i.e. it will generate a different series of random numbers each
   * time the program is run.
   */
  Random();

  /** Copy constructor.
   *  Initializes/sets the random number generator to exactly
   *  the same state as the argument, i.e. both will generate exactly
   *  the same series of random numbers from then on.
   */
  Random(const Random&);

  /** Copy operator.
   *  Initializes/sets the random number generator to exactly
   *  the same state as the argument, i.e. both will generate exactly
   *  the same series of random numbers from then on.
   */
  Random& operator=(const Random&);

  /** 
   * Construct with seed.
   *  Initializes the random number generator deterministically
   *  using a single uint as the 'seed'. A linear congruential
   *  generator is used to generate the 37 uints needed
   *  as the real seed. The same seed will produce the
   *  same series of random numbers.
   *
   *  9667566  is a good seed.
   */
  Random(unsigned int seed);

  /** Construct with seed.
   *  Initializes the random number generator deterministically
   *  using 37 uints as the 'seed'. The same seed will
   *  produce the same series of random numbers.
   */
  Random(unsigned int seed[vnl_random_array_size]);

public:

  /** 
   * Starts a new non-deterministic sequence from an already declared generator.
   */
  void reseed();

  /** 
   * Starts a new deterministic sequence from an already declared generator using the provided seed.
   */
  void reseed(unsigned int);

  /** 
   * Starts a new deterministic sequence from an already declared generator using the provided seed.
   */
  void reseed(unsigned int[vnl_random_array_size]);

  /** 
   * This restarts the sequence of random numbers.
   *  Restarts so that it repeats
   *  from the point at which you declared the generator, last
   *  initialized it, or last called a 'reseed'.
   */
  void restart();

public:
  // Generates a random int.
  int nextInt() const;

  // Generates a random  int in [a,b]
  int nextInt(int a, int b) const;

  // Generates a random int in [0,b]
  int nextInt(int b) const{return nextInt(0, b);}

  // Generates a random int in [a,b]
  int nextInt(int a, int b, int& count) const;

//------------------------------------------------------------------
//------------------------------------------------------------------

  // Generates a random XLONG.
  XLONG nextLong() const;

  // Generates a random float.
  float nextFloat() const;

  // Generates a random double.
  double nextDouble() const;

protected:

  unsigned int  linear_congruential_int();
  int next(int bits) const;

protected:

  enum {linear_congruential_multiplier = 1664525, mz_previous1 = 24};

  mutable unsigned int linear_congruential_previous;
  mutable unsigned int mz_seed_array[vnl_random_array_size];
  mutable unsigned int mz_array[vnl_random_array_size];

  mutable int mz_array_position;
  mutable int mz_borrow;
};

__OPENCODE_END_NAMESPACE

#endif


