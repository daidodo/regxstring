# regxstring
This is a **Random String Generator** based on **Regular Expression**.

The idea is simple: Given a regular expression, generate random strings that can match it.

For example, to generate 5 random IP addresses that have all *same* components, we can use this:
```
 $ echo "^(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(?:\.\1){3}$" | ./regxstr 5
 250.250.250.250
 213.213.213.213
 4.4.4.4
 118.118.118.118
 2.2.2.2
```
No surprising, the above regular expression matches an IP address with 4 same dot parts.
Isn't it cool? 

*regxstring* can do much more. You can try more complex regular expressions. Basically, most Perl 5 supported regular expressions are also supported by *regxstring*, as showing bellow:
```
Meta-character(s)   Description
--------------------------------
\                   Quote the next meta-character
^                   Match the beginning of the line
$                   Match the end of the line (or before newline at the end)
?                   Match 1 or 0 times
+                   Match 1 or more times
*                   Match 0 or more times
{n}                 Match exactly n times
{n,}                Match at least n times
{n,m}               Match at least n but not more than m times
.                   Match any character (except newline)
(pattern)           Grouping
(?:pattern)         This is for clustering, not capturing; it groups sub-expressions like "()", but doesn't make back-references as "()" does
(?=pattern)         A zero-width positive look-ahead assertion, e.g., \w+(?=\t) matches a word followed by a tab, without including the tab
(?!pattern)         A zero-width negative look-ahead assertion, e.g., foo(?!bar) matches any occurrence of "foo" that isn't followed by "bar"
|                   Alternation
[xyz]               Matches a single character that is contained within the brackets
[^xyz]              Matches a single character that is not contained within the brackets
[a-z]               Matches a single character that is in a given range
[^a-z]              Matches a single character that is not in a given range
\f                  Form feed
\n                  Newline
\r                  Return
\t                  Tab
\v                  Vertical white space
\d                  Digits, [0-9]
\D                  Non-digits, [^0-9]
\s                  Space and tab, [ \t\r\n\f]
\S                  Non-white space characters, [^ \t\r\n\f]
\w                  Alphanumeric characters plus '_', [0-9a-zA-Z_]
\W                  Non-word characters, [^0-9a-zA-Z_]
\N                  Matches what the Nth marked sub-expression matched, where N is a digit from 1 to 9
```
One more notice, *regxstring* also includes a library that allows you to generate such strings in your C/C++ programs.

Please enjoy yourself!
