#include "port/librandom.h" 
#include "port/libsys.h"
#include "port/libnumber.h"

__OPENCODE_BEGIN_NAMESPACE

Random::~Random()
{
  for (int i=0;i<vnl_random_array_size;++i)
  {
    mz_seed_array[i] = 0;
    mz_array[i] = 0;
  }
}

Random::Random(unsigned int seed)
: mz_array_position(0), mz_borrow(0)
{reseed(seed);}

Random::Random(unsigned int seed[vnl_random_array_size])
: mz_array_position(0), mz_borrow(0)
{
  reseed(seed);
}

Random::Random(const Random& r)
: linear_congruential_previous(r.linear_congruential_previous)
, mz_array_position(r.mz_array_position)
, mz_borrow(r.mz_borrow)
{
  for (int i=0;i<vnl_random_array_size;++i)
  {
    mz_seed_array[i] = r.mz_seed_array[i];
    mz_array[i] = r.mz_array[i];
  }
}

Random& Random::operator=(const Random& r)
{
  linear_congruential_previous=r.linear_congruential_previous;
  mz_array_position=r.mz_array_position;
  mz_borrow=r.mz_borrow;
  for (int i=0;i<vnl_random_array_size;++i)
  {
    mz_seed_array[i] = r.mz_seed_array[i];
    mz_array[i] = r.mz_array[i];
  }
  return *this;
}

Random::Random() 
: mz_array_position(0), mz_borrow(0)
{
  reseed();
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------

void Random::reseed()
{
  reseed((unsigned int)(System::currentTimeMillis()/1000));
}

void Random::reseed(unsigned int seed)
{
  mz_array_position = 0;
  mz_borrow = 0;

  linear_congruential_previous = seed;
  // Use the lc generator to fill the array
  for (int i=0;i<vnl_random_array_size;++i)
  {
    mz_seed_array[i] = linear_congruential_int();
    mz_array[i] = mz_seed_array[i];
  }

  // Warm up with 1000 randoms
  for (int j=0;j<1000;j++) 
  { 
    nextInt();
  }
}

void Random::reseed(unsigned int seed[vnl_random_array_size])
{
  mz_array_position = 0;
  mz_borrow = 0;

  for (int i=0;i<vnl_random_array_size;++i)
  {
    mz_array[i] = seed[i];
    mz_seed_array[i] = seed[i];
  }
}

void Random::restart()
{
  mz_array_position = 0;

  for (int i=0;i<vnl_random_array_size;++i)
  {
    mz_array[i] = mz_seed_array[i];
  }
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------

int Random::nextInt() const
{
  return next((int)(sizeof(int) * 8));
}

int Random::nextInt(int lower, int upper) const
{
  int count = 1;
  return nextInt(lower,upper,count);
}

int Random::nextInt(int lower, int upper, int& count) const
{

  // Note: we have to reject some numbers otherwise we get a very slight bias
  // towards the lower part of the range lower - upper. Hence this is a "count"
  // version of the above function that returns the number of nextInt()
  // calls made.

  unsigned int range = upper-lower+1;
  unsigned int denom = (unsigned int)(Number::ULONG_MASK/range);
  unsigned int ran;
  count = 1;

  while ((ran = (unsigned int)(nextInt())) >= denom*range) ++count;

  return lower + int(ran/denom);
}

XLONG Random::nextLong() const
{
  return (((XLONG)nextInt()) << (sizeof(int) * 8)) + nextInt();
}

float Random::nextFloat() const
{
  int i = next(24);
  return i / ((float)(1 << 24));
}

double Random::nextDouble() const
{
  XLONG l = ((XLONG)(next(26)) << 27) + next(27);
  XLONG oneL = 1;
  return l / (double)( oneL << 53);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------

unsigned int Random::linear_congruential_int()
{
  linear_congruential_previous = (unsigned int)((linear_congruential_previous*linear_congruential_multiplier + 1) & Number::ULONG_MASK);
  return linear_congruential_previous;
}

int  Random::next(int bits) const
{
  unsigned int p1 = mz_array[(vnl_random_array_size + mz_array_position - mz_previous1)%vnl_random_array_size];
  unsigned int p2 = (unsigned int)( (p1 - mz_array[mz_array_position] - mz_borrow) & Number::ULONG_MASK);

  if (p2 < p1) mz_borrow = 0;
  if (p2 > p1) mz_borrow = 1;

  mz_array[mz_array_position] = p2;
  mz_array_position = (++mz_array_position)%vnl_random_array_size;

  return (int)Number::ushiftRight(p2 ,(int)(sizeof(int)*8 - bits));
}

__OPENCODE_END_NAMESPACE

