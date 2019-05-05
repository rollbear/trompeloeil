# Contribution Guidelines and Requirements <!-- omit in toc -->

This document covers contribution guidelines that should be followed if you plan
to open a pull request to this repository. Please refer to the table of contents
below to identify the area of change you plan to make (documentation, code,
etc).

* [Documentation (Markdown)](#documentation-markdown)
  * [Line Wrapping](#line-wrapping)
  * [Formatting Tables](#formatting-tables)
  * [Table of Contents](#table-of-contents)
  * [Markdown Linting](#markdown-linting)
  * [Forcing Line Breaks](#forcing-line-breaks)

## Documentation (Markdown)

When editing documentation, please use [Visual Studio Code][vscode]. The reason
this is recommended is because of the extensions that assist in editing markdown
files. By using VS Code along with our recommended extensions, you can ensure
that the process of accepting any documentation changes via pull request will go
more smoothly.

Please install the following extensions:

* [Rewrap](https://marketplace.visualstudio.com/items?itemName=stkb.rewrap)
* [Markdown All in One](https://marketplace.visualstudio.com/items?itemName=yzhang.markdown-all-in-one)
* [markdownlint](https://marketplace.visualstudio.com/items?itemName=DavidAnson.vscode-markdownlint)

> **Note**: All guidelines in this section assume you're using VS Code for
> making changes. If you are using different tooling, please adapt the
> instructions as best you can.

[vscode]: https://code.visualstudio.com/

### Line Wrapping

The workspace settings are configured to show rulers at column 80. Hard wraps
are expected at column 80 as well, to keep format consistent and easy to read.
To assist with wrapping, please use the Rewrap extension. Simply select a range
of text or put your caret inside of a paragraph and press `Alt + Q` (default key
mapping) to wrap the text automatically for you.

Not everything is wrapped. For example, bullet lists of URLs are difficult to
format. Same with markdown tables. So it's OK to leave some things past 80 if it
makes sense. The guideline is intended mostly for written paragraphs.

> **Note**: Doing *Format Document* in Markdown files does not automatically
> hard-wrap for you.

### Formatting Tables

Please use Markdown All in One (*Format Document* or *Format Selection*) to
format tables.

### Table of Contents

Some markdown files use manual table of contents (using HTML elements). For
legacy reasons, these are excluded. But some newer markdown files (like this
one) use the *Markdown All in One* extension to update table of contents. The
workspace VS Code settings have this extension configured to update the TOC when
saving the document. So it should be done automatically for you.

You can update the TOC manually at any time by selecting *Markdown: Update Table
of Contents* from the VS Code Command Palette.

### Markdown Linting

Certain markdown linting rules are configured for this project. If you're using
VS Code as instructed with the *markdownlint* extension, you should see
interactive squigglies as you type that should help guide you to correcting any
Markdown-specific formatting issues that will likely get raised in a pull
request. If you resolve all lint issues before you submit your PR, things will
go more smoothly.

Explanation of lint rules for this project is below.

* [MD004]: Require asterisks (`*`) in unordered lists.
* [MD013]: Ignore line length for code blocks and tables.
* [MD024]: Allow duplicate headings.
* [MD026]: Allow trailing `?` in heading (For the FAQ).
* [MD033]: Allow certain HTML elements in documents.

[MD004]: https://github.com/DavidAnson/markdownlint/blob/master/doc/Rules.md#md004
[MD013]: https://github.com/DavidAnson/markdownlint/blob/master/doc/Rules.md#md013
[MD024]: https://github.com/DavidAnson/markdownlint/blob/master/doc/Rules.md#md024
[MD025]: https://github.com/DavidAnson/markdownlint/blob/master/doc/Rules.md#md025
[MD026]: https://github.com/DavidAnson/markdownlint/blob/master/doc/Rules.md#md026
[MD033]: https://github.com/DavidAnson/markdownlint/blob/master/doc/Rules.md#md033

### Forcing Line Breaks

Markdown syntax to force line breaks is to use two trailing spaces at the end of
a line. However, some tools trim trailing whitespace (because 99% of the time
it's not wanted). To avoid this ambiguity, please use the `<br>` HTML element at
the end of your line instead of two spaces. Example:

```md
This is the first line.<br>
This is the second line.
```
