# Bug 1

## A) How is your program acting differently than you expect it to?
- When we run out tests for CheckDocTable we get a fatal glibc error
is sysmalloc where we are failing a few malloc assertion tests
and getting NULL terminators.

## B) Brainstorm a few possible causes of the bug
- Since this is a malloc failure we most likely aren't freeing
somewhere in if and else loops in one of our methods.
- We think it's possible that our FileParser is not freeing
our words correctly or it's possible that we have another bug in
CrawlFileTree such as not freeing our memory properly in one of
steps.
- Since we have so much code in different areas it's also possible
that we have another bug that prompts this new bug to arise which
is possible since our CrawlFileTree is using a lot of other calls
so maybe we got one function mixed up with another there.

## C) How you fixed the bug and why the fix was necessary
- We stepped through our code and checked FileParser first since
that seemed like the area we were most unsure about. The bug
was actually in FileParser.c where we forgot to add 1
when we're mallocing our word. It sounds like something
pretty simple but for some reason this actually
took us a while to find where the issue was. This fix was
necessary so that we could pass our tests and
it was good that we ran into this bug early since we
realized that there was a few other areas that utilized 
this same idea and caused us to have other errors there
as well.


# Bug 2

## A) How is your program acting differently than you expect it to?
- When we run ./test_suite we get a segmentation fault at 
Test_CrawlFileTree.ReadsFromDisk and we can't pinpoint exactly where
the issue is coming from. The thing with segfaults is that we 
don't get a specific place where we're failing the tests so we
can't exactly immediately be able to have an idea of where the bug is.

## B) Brainstorm a few possible causes of the bug
- Maybe we aren't doing NULL checks in CrawlFileTree
properly? Or our if loops are incorrect and checking for
the wrong things?
- We feel like it's in CrawlFileTree with one of our continue
or return statements where we are continuing when we shouldn't 
or returning when we shouldn't.
- We were wondering if the bug could also be FileParser of step 6
where our while loops are incorrect and that the conditions for when
we return or update our currentptr aren't being entered and executed 
properly.

## C) How you fixed the bug and why the fix was necessary
- The bug was actually in CrawlFileTree in one of our for loops.
Essentially what was happening was that we weren't ignoring
when the directory entry name was . or .. as we didn't decrement
our for loop to factor out the iteration. For some reason,
finding this fix took longer than expected since it was such a small
issue that we had and we just kept overlooking it when we were
trying to figure out where our bug was. This fix was ultimately
necessary so that then we would be able to properly ignore
the invalid directory entry name rather than just continuing
our loop without actually ignoring it.


# Bug 3

## A) How is your program acting differently than you expect it to?
- When we try to run our program for query results, we get a double
free or corruption error in search shell which isn't supposed to happen
since when we input a query we should be getting results for it instead.

## B) Brainstorm a few possible causes of the bug
- We think it's possible that we are freeing once in our
while loop and then freeing another time after our while loop.
- Maybe we freed in our helper function and then freed again 
for the same thing after we call our helper function when 
we actually didn't need to.
- Could be having errors with our strtok_r or other LLIterator
functions calls where they might've already allocated or deallocated
memory and we're getting ourselves mixed up.

## C) How you fixed the bug and why the fix was necessary
- We fixed the bug by examining our while loop and find out that 
searchshell in main is freeing our words and then in our helper 
method GetNextLine, we're freeing our ret_str which in this case
we called it and passed our words as ret_str in GetNextLine which
causes us to free twice. We changed our code in GetNextLine to not
free our ret_str and it fixed the bug. This fix was necessary so
that we wouldn't have a double free/corruption error so that
we can get some form of query results printed back to us after we
enter user input.
