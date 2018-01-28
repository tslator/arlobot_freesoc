#ifndef CONTRACT_H
#define CONTRACT_H

/* NASSERT macro disables all contract validations
   (assertions, preconditions, postconditions, and invariants).
*/
#ifdef NASSERT /* NASSERT defined--DbC disabled */

#define DEFINE_THIS_FILE
#define ASSERT(ignore_) ((void)0)
#define ALLEGE(test_) ((void)(test_))

#else /* NASSERT not defined--DbC enabled */

#ifdef __cplusplus
extern "C"
{
#endif

/* callback invoked in case of assertion failure */
void on_assert(char const *file, unsigned line);

#ifdef __cplusplus
}
#endif

/* Compared to the standard assert(), the macro ASSERT() conserves memory (typically ROM) by
   passing THIS_FILE__ as the first argument to onAssert__(), rather than the
   standard preprocessor macro __FILE__. This avoids proliferation of the multiple copies of
   the __FILE__ string but requires invoking macro DEFINE_THIS_FILE, preferably at the
   top of every C/C++ file.
*/
#define DEFINE_THIS_FILE static char const THIS_FILE__[] = __FILE__

#define ASSERT(test_) ((test_) ? (void)0 : on_assert(THIS_FILE__, __LINE__))
#define ALLEGE(test_) ASSERT(test_)

#endif /* NASSERT */

#define REQUIRE(test_) ASSERT(test_)
#define ENSURE(test_) ASSERT(test_)
#define INVARIANT(test_) ASSERT(test_)

#endif