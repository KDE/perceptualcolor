<!--
SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
SPDX-License-Identifier: BSD-2-Clause OR MIT
-->

# Contributing to PerceptualColor

## Merge requests

Merge requests can be done
[at KDE’s Gitlab instance](https://invent.kde.org/libraries/perceptualcolor/).

This project uses CI to perform some checks on merge requests. You can also
run locally most of these checks with
```shell
scripts/local-ci-fast.sh
scripts/local-ci-build-warnings.sh
```
and with

```shell
scripts/format.sh
```
you can to some adjustments automatically. Please run also
```shell
scripts/update-screenshots.sh
```
before each merge request to update the screenshots. This is currently not
enforced by the CI, but nevertheless important.

## Local development

```shell
scripts/local-ci-fast.sh
```

will also create API documentation. The folder `publicapi` corresponds to the
[Online API documentation](https://api.kde.org/perceptualcolor/html/index.html)
and contains only the public API. The folder `publicapiandinternals` however
contains a complete documentation, including private members and private
classes, which is very useful when working on PerceptualColor.