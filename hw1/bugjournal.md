# Bug 1

## A) How is your program acting differently than you expect it to?
- Our next node is pointing to NULL which is different from the 
expected value so we aren't passing on of our LinkedList tests.
When I try fixing it, I run into a lot of seg faults which makes
me think it's possible we are using the wrong helper functions in our
code as well since I drew out the code and I can't find any glaring
issues.

## B) Brainstorm a few possible causes of the bug
- Somewhere in our LLIterator_Remove one of our pointers is pointing
to null when it shouldn't.
- Maybe we aren't actually entering one of else if cases correctly
 (wrote some testing code for this and our else if cases are fine)
- We think that it's possible that we have one of our methods confused
with another somewhere so this one wrong call is creating all of these
bugs down the road.

## C) How you fixed the bug and why the fix was necessary
- One part of the issue is that we weren't freeing our iterator correctly.
We initially did LLIterator_Free() rather than free() so that 
caused a few issues in our code. Once we fixed those, most of our issues
were slowly resolved by doing small changes to our code here and there. Another thing we did was to go inside the test_linkedlist.cc to get an idea of which tests were failing and sort of backwards find out where exactly in our code could be causing this issue.

# Bug 2

## A) How is your program acting differently than you expect it to?
- One of our test for HashTable kept failing. Our HashTable_Find 
method kept having the wrong old key values. For some reason,
it was always a really long number which was different than 
expected.

## B) Brainstorm a few possible causes of the bug
- We're updating our key_value value incorrectly so we
always get the wrong value.
- It's possible that we don't even need to be updating
our key_values' so we're doing unneccessary work.
- We're calling our hasKey helped method so it's possible that
something is wrong with out helper which is causing bugs
in our HashTable_Find method now.

## C) How you fixed the bug and why the fix was necessary
- The fix in the end was that we didn't need to be updating
our keyvalue's so essentially we were doing too much in our method.
This fix was neccessary so that we could have the correct
keyvalue value before and after our insert calls in the tests
given because otherwise it would've failed.


# Bug 3

## A) How is your program acting differently than you expect it to?
- After completing all of the tests for LinkedList and HashTable,
we ended up with a lot of memory issues. There was a lot of 
bytes in blocks that were either directly lost or indirectly lost.

## B) Brainstorm a few possible causes of the bug
- It's likely we forgot to free our iterator chain in HashTable.
- We think there's a few issues in LinkedList where we aren't 
freeing our nodes properly.
- We also probably aren't freeing out keyvalue in HashTable.

## C) How you fixed the bug and why the fix was necessary
- There was a lot of fixes needed for this. It took a long time
to go through everything that we forgot to free. We fixed this by
looking through our code and finding areas that we thought we
might've forgotten to free the node or the keyvalue or the 
chain iterator. This fix was necessary since our valgrind test 
would result in a lot of leaks and wouldn't pass otherwise. We
ended up not completely getting rid of the memory issues. Towards
the end a lot of time when we tried to rewrite the code
to get rid of the bug, we would end up breaking our code and our
test would start failing. What ended up being our "fix" was that 
we were using helper methods like splice and append that were already
freeing so we were essentially double freeing so it gave us a lot of
errors. Once we understood more about why and where we were freeing,
we were a lot better at finding exactly where the memory issues were at
and coming up with a solution.
