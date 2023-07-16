# Bug 1

## A) How is your program acting differently than you expect it to?
- After finished up part B and ran valgrind on the test suite,
we ended up having memory leaks where the tests stopped running at 
DocIDTableReader and we get a lot of errors saying conditional
jump or move depends on unitialized value.

## B) Brainstorm a few possible causes of the bug
- Since the tests stopped running at DocIDTableReader, we're
currently thinking that's where our bugs are. One possible cause
could be that we're
- We used ntolh to host for Step 2 so it's possible that might
be causing issues when we should be actually using ToHostFormat()
instead. 
- Since we only ran valgrind after we completed part B it's
possible that we have bugs in other places and DocIDTableReader
is simply the trigger for it but not where the bug actually is so
the bug could be in other areas of our code such as HashTableReader
but we feel like this is unlikely since it wouldn't have passed their
tests if something was wrong with the code in another method.

## C) How you fixed the bug and why the fix was necessary
- We found the issue in Step 6 of DocIDTableReader where we
were seeking to the element before we read the next element 
position from the bucket header. Once we rearranged our code
we were able to fix the memory leaks and conditional jump error
messages. Additionally, we also changed ntolh to ToHostFormat()
since using ntolh in that context isn't correct and ToHostFormat()
would be better so this fix was kind of a two in one bug that
we fixed.

# Bug 2

## A) How is your program acting differently than you expect it to?
- Our output when we run ./filesearchshell unit_test_indices/* to
test if it correctly gives us the file path and the ranking
is out of order where the rankings are not in the 
order that we want.

## B) Brainstorm a few possible causes of the bug
- We think that we did something wrong in QueryProcessor
because that's where we're actually geting the list of doc ids
with the ranks that contain word. We think specifically,
it's where we're looping through and processsing each doc_id
and we're counting it incorrectly.
- Since it's also giving us the wrong file paths, we think
we're also incorrectly checking if the doc_id is in our 
list or not and we're adding it to it when we're not
supposed to.
- Another possible bug could be we had a bug in another
area of our code as there is a lot of moving parts
in QueryProcessor and it could be that we did something wrong
somewhere else and now it's an issue when we're running our
./filesearchshell but we're going to first investigate
the first two causes of the bug before we get to this one
since we think it's not as likely.

## C) How you fixed the bug and why the fix was necessary
- We fixed the bug when we realized that since we were using
push_back again at the very end after we processed everything.
Essentially since we did this, everything was pushed back to the end again
of the results vector so that what we wanted in the beginning were actually at 
the end because of the second push_back into results.
After we figured this our, the fix for this bug was 
easier to tackle where instead of using push_back we just processed it directly
into results vector and took out the second push_back which fixed our issues.
This fix was necessary so that our output when we run 
./filesearchshell unit_test_indices/* to find the file path and ranking
for the inputted queries is in the correct order for how we want
everything to be sorted and displayed.


# Bug 3

## A) How is your program acting differently than you expect it to?
- When we run valgrind with ./filesearchshell unit_test_indices/* 
and input a few words and then ctrl D we were getting memory leaks
when we actually want there to be no leaks possible.

## B) Brainstorm a few possible causes of the bug
- Since in our helper method QueryWordProcessor we're calling
ProcessQuery, it could be that we have a bug in our QueryProcessor
where we actually implemented ProcessQuery. Since there's a lot of 
moving parts in QueryProcessor, it could be that we forgot to call
delete somewhere and now we're getting memory leaks in filesearchshell.
- We made two helper methods for our filesearchshell so it could be 
that one of these helper methods isn't properly freeing memory when
it's supposed to or that it's doing something that it shouldn't.
Maybe we wrote some code that should be in the main method or
is being called multiple time in the while loop when it should
simply be called once.
- Maybe we misunderstood how to implement filesearchshell and we
missed something that we were supposed to do or that we've called
a method or did something we shouldn't so it's causing unexpected 
behavior and memory leaks.

## C) How you fixed the bug and why the fix was necessary
- We reorganized our code and just put everything in the main method
and tested it to see if it's possible the way that we broke down
the tasks into helper methods is what is causing the leaks and it 
turned out that there were no memory leaks when we did that. Once we
narrowed it down to that being the cause of our bug,
we realized that since we were taking in user input
in one of our helper methods rather than in the main method and then error checking main
that was what was causing the memory leaks. We fixed this by passing in the input
from the user to our helper method instead rather than taking in input 
in the helper method. Then, when we ran it again and our memory issues were fixed.
