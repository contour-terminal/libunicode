
# Processing UTF-8 byte sequences into grapheme clusters

## The Objective

👪👪👪

- Process a consecutive sequence text in UTF-8 encoding into a group of grapheme clusters
- Stop processing on one of the conditions:
  - end of input stream is reached
  - a control character (such as newline or escape character) has been found
  - the maximum number of grapheme clusters in narrow width (aka. page width) have been consumed (while wide characters count as two narrow characters)
- Allow resuming processing text when we previously stopped in the middle of a grapheme cluster
- The algorithm must be as resource efficient as possible:
  - do not require any dynamic memory allocations during text processing
  - reduce instruction branching as much as possible
  - utilize SIMD to improve throughput performance
- Invalid codepoints are treated with east asian width Narrow (1 column)
- The event emitting mechanism must be zero-overhead. If these events are not needed, they must not penalize performance.

## Corollaries

- When reaching the end of the input, but a single UTF-8 sequence has not been fully processed yet, no event will be emitted. The subsequent call to process() will resume UTF-8 decoding.
- When no grapheme boundary has been found while the end of input has been reached, no grapheme cluster will be reported. The subsequent calls that detect the boundary will report the **full** grapheme cluster from the sum of all calls.

## Implementation

Scanning US-ASCII can be easily implemented using SIMD, increasing scanning performance dramatically.

Scanning non-US-ASCII text, complex Unicode codepoints, is way more complex, because more depth is involved.

In order to reliably stop scanning at the page width - we must take into account
that the character we see on the screen is not necessarily just a single byte,
nor even a single UTF-32 codepoint, but rather a sequence of UTF-32 codepoints.
This is what we call **grapheme cluster**. A grapheme cluster is a user perceived single grapheme entity,
that can be one or more Unicode codepoints.

We therefore must be able to determine the border of when a grapheme cluster ends and the next one begins.

Because scanning US-ASCII text can be implemented using SIMD but complex Unicode cannot, we split both
tasks into their own sub tasks, and then alter between the two in order to scan the sum of all Unicode text.

In this article, we'll befocusing on scanning for complex Unicode.

We also must be able to suspend and resume scanning text at any arbitrary point
in time, because we are not guaranteed to always have all bytes available in a single call.

## Example Processing: Family Emoji

```
UTF-8  |  F0 9F 91 A8 | E2 80 8D |  F0 9F 91 A9 | E2 80 8D |  F0 9F 91 A7 | E2 80 8D |  F0 9F 91 A6
UTF-32 | U+1F468 (👨) |   U+200D | U+1F469 (👩) |   U+200D | U+1F467 (👧) |   0x200D | U+1F466 (👦)
GC     | 👨‍👩‍👧‍👦
```

## Events

### On complete valid UTF-32 codepoint

- remember byte pointer to mark start of next Unicode codepoint
- if last codepoint and next codepoint are GC breakable: then report grapheme cluster

### On complete grapheme cluster

- assert: last and next valid codepoint are GC breakable
- if the new grapheme cluster fits into the page width, then report grapheme cluster
- otherwise, remember that GC range and terminate


## Test cases

- Have a sequence of 2 wide emoji (e.g. U+1F600), but only a page width of 3. We can only align one emoji per line.
