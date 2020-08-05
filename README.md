<img src="https://i.imgur.com/BL6J8Jh.png" width="500">

[![Financial Contributors on Open Collective](https://opencollective.com/ossia/all/badge.svg?label=financial+contributors)](https://opencollective.com/ossia) [![join the chat at https://gitter.im/ossia/score](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/ossia/score?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge) [![Downloads](https://img.shields.io/github/downloads/ossia/score/total.svg)](https://github.com/ossia/score/releases) [![Version](https://img.shields.io/github/release/ossia/score.svg)](https://github.com/ossia/score/releases)

[**ossia score**](http://ossia.io) is a ***sequencer*** for audio-visual artists, designed to create ***interactive*** shows. 

Sequence **OSC, MIDI, DMX, sound files** and more, between multiple software and hardware, create ***interactive*** and ***intermedia*** scores and ***script*** in JavaScript to customize your score.

Free, open source and runs on desktop, mobile and embedded.

Read more on http://ossia.io, come ask questions on the [forum](http://forum.ossia.io/c/score) or hang out in a [chat](https://gitter.im/ossia/score) with us, we will be happy to help you !

### Download the [latest release here](https://github.com/ossia/score/releases/) and read the Releases section in this README.

![ossia score screenshot](/docs/score.png?raw=true)

## Releases

Releases are available for [Windows, Linux (via AppImage) and Mac OS X](https://github.com/ossia/score/releases/latest).

* Windows: install and run.
* OS X: extract and run ossia score.app.
* Linux: make executable (right click -> permissions or `chmod +x`) and run the AppImage.

## Build status
* Linux, OS X: [![Travis Status](https://travis-ci.org/ossia/score.svg?branch=master)](https://travis-ci.org/ossia/score)
* Windows: [![Appveyor Status](https://ci.appveyor.com/api/projects/status/github/ossia/score?branch=master&svg=true)](https://ci.appveyor.com/project/JeanMichalCelerier/score)
* [![OpenHub](https://www.openhub.net/p/score/widgets/project_thin_badge?format=gif)](https://www.openhub.net/p/score)
* ossia score uses [CppDepend](https://www.cppdepend.com/) to ensure consistent code quality improvements ; please check it out !

In order to build score, follow the [instructions](https://github.com/ossia/score/wiki/Build-and-install).
Current builds tested on Mac OS X, Ubuntu 14.04, ArchLinux, Windows 8.1.

## Contributing

When updating the score repository through the command line, don't forget to update the submodules, they change often.

    cd score
    git pull
    git submodule update --init --recursive

Some useful and easy potential contributions are listed [on the website](http://ossia.io/contributing/).

There are also many fixable areas in the code :
sequencer
* [**TODO**](https://github.com/ossia/score/search?q=TODO) : for general problems. Some are [five-minute fixes](https://github.com/ossia/score/blob/2e393a1786154c11d766e6c6476cc2bd5faa95d0/base/plugins/iscore-lib-process/Process/Style/ScenarioStyle.cpp#L3), other are [a multiple-day quest](https://github.com/ossia/score/blob/2e393a1786154c11d766e6c6476cc2bd5faa95d0/base/lib/core/plugin/PluginDependencyGraph.hpp#L67)
* [**FIXME**](https://github.com/ossia/score/search?q=FIXME) : for critical bugs / problems.
* [**MOVEME**](https://github.com/ossia/score/search?q=REMOVEME) : when something is not where it should be.
* [**REMOVEME**](https://github.com/ossia/score/search?q=REMOVEME) : when something is redundant.
* [**RENAMEME**](https://github.com/ossia/score/search?q=RENAMEME) : when the class does not match with the file it's in.
* [**OPTIMIZEME**](https://github.com/ossia/score/search?q=OPTIMIZEME) : when an easy-to-write-but-suboptimal algorithm is used.

Finally, one can write its own score plug-ins to add custom features to the software.
An example plug-in with the most common classes reimplemented is provided [here](https://github.com/ossia/iscore-addon-tutorial).

## Contributors

### Code Contributors

This project exists thanks to all the people who contribute. [[Contribute](CONTRIBUTING.md)].
<a href="https://github.com/ossia/score/graphs/contributors"><img src="https://opencollective.com/ossia/contributors.svg?width=890&button=false" /></a>

### Financial Contributors

Become a financial contributor and help us sustain our community. [[Contribute](https://opencollective.com/ossia/contribute)]

#### Individuals

<a href="https://opencollective.com/ossia"><img src="https://opencollective.com/ossia/individuals.svg?width=890"></a>

#### Organizations

Support this project with your organization. Your logo will show up here with a link to your website. [[Contribute](https://opencollective.com/ossia/contribute)]

<a href="https://opencollective.com/ossia/organization/0/website"><img src="https://opencollective.com/ossia/organization/0/avatar.svg"></a>
<a href="https://opencollective.com/ossia/organization/1/website"><img src="https://opencollective.com/ossia/organization/1/avatar.svg"></a>
<a href="https://opencollective.com/ossia/organization/2/website"><img src="https://opencollective.com/ossia/organization/2/avatar.svg"></a>
<a href="https://opencollective.com/ossia/organization/3/website"><img src="https://opencollective.com/ossia/organization/3/avatar.svg"></a>
<a href="https://opencollective.com/ossia/organization/4/website"><img src="https://opencollective.com/ossia/organization/4/avatar.svg"></a>
<a href="https://opencollective.com/ossia/organization/5/website"><img src="https://opencollective.com/ossia/organization/5/avatar.svg"></a>
<a href="https://opencollective.com/ossia/organization/6/website"><img src="https://opencollective.com/ossia/organization/6/avatar.svg"></a>
<a href="https://opencollective.com/ossia/organization/7/website"><img src="https://opencollective.com/ossia/organization/7/avatar.svg"></a>
<a href="https://opencollective.com/ossia/organization/8/website"><img src="https://opencollective.com/ossia/organization/8/avatar.svg"></a>
<a href="https://opencollective.com/ossia/organization/9/website"><img src="https://opencollective.com/ossia/organization/9/avatar.svg"></a>
