harbour-callrecorder
====================

Native call recorder for Jolla's SailfishOS.

**WARNING!!! This is application requires unstable version of SailfishOS.**

**Table of Contents**

- [harbour-callrecorder](#user-content-harbour-callrecorder)
	- [Requirements](#user-content-requirements)
	- [Installation from RPM](#user-content-installation-from-rpm)
	- [Installation from sources](#user-content-installation-from-sources)
	- [Troubleshooting](#user-content-troubleshooting)
		- [The calls are not recorded](#user-content-the-calls-are-not-recorded)
		- [The UI application shows white screen](#user-content-the-ui-application-shows-white-screen)
	- [FAQ](#user-content-faq)

##Requirements

* SailfishOS 1.1 or later
* Allowance for unrusted software installation

##Installation from RPM

1. Enable untrusted software installation at Settings -> Untrusted software.
2. Download the latest version RPM package which is located at https://dpurgin.github.io/. This page is best accessed from your Jolla device. The package you need *doesn't* say `debuginfo` or `debugsource` in its name.
3. If you have downloaded the package with the Jolla Browser, go to Settings -> Transfers and tap on the downloaded package, it prompts for installation after a while.
4. Tap on `Install` when prompted.
5. After installation is successful, a notification appears.
6. Make sure `Call Recorder` has appeared in the list of applications.
6. Reboot the phone.
7. Make a call and check if the recording has appeared in `Call Recorder` application. If it hasn't, go to Troubleshooting section.

You don't need to have the UI application running all the time to have your calls recorded. They always are as soon as the service is enabled. 

##Installation from sources

This section is WIP, but it's basically `git clone`, make and deploy with latest SDK. You might need to install flac, pulse and qofono-qt5 development packages on the build VM, just follow the build errors.

##Troubleshooting

### The calls are not recorded

Did you reboot your phone after installation? If not, reboot and try again. If still not working, please check if the service is enabled and running. You will need the Terminal application. Issue the following command in terminal (mind 'd' at the end and you don't need to enter $ sign, it's already there):

```
$ systemctl --user status harbour-callrecorderd
```

If the output looks like this after reboot (mind `disabled` at the second line):

```
harbour-callrecorderd.service - Call Recorder Daemon
   Loaded: loaded (/usr/lib/systemd/user/harbour-callrecorderd.service; disabled)
   Active: inactive (dead)
```

then try (mind `d` at the end):

```
$ systemctl --user enable harbour-callrecorderd
$ systemctl --user start harbour-callrecorderd
```

If the service is up and running, it should say something like this:

```
harbour-callrecorderd.service - Call Recorder Daemon
   Loaded: loaded (/usr/lib/systemd/user/harbour-callrecorderd.service; enabled)
   Active: active (running) since Mon 2014-10-27 00:02:12 ALMT; 19s ago
 Main PID: 1136 (harbour-callrec)
   CGroup: /user.slice/user-100000.slice/user@100000.service/harbour-callrecorderd.service
           └─1136 /usr/bin/harbour-callrecorderd
```

### The UI application shows white screen

Check if you have SailfishOS 1.1 (Settings -> Sailfish OS updates).

##FAQ

**Q:Does it require developer mode?**<br>
A: Generally, no. If you run into trouble, you might need the Terminal application to diagnose. 

