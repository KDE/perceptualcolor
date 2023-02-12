<!--
SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
SPDX-License-Identifier: BSD-2-Clause OR MIT
-->

# Contribution to PerceptualColor

This project uses CI to perform some checks on merge requests. You can also run locally most of these checks with

```shell
scripts/local-ci-fast.sh
scripts/local-ci-build-warnings.sh
```

and with

```shell
scripts/automatic-integration.sh
```

you can to some adjustments automatically.

Please run also

```shell
scripts/update-screenshots.sh
```shell

before each merge request to update the screenshots. This is currently not enforced by the CI, but nevertheless important.