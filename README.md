# Pocketbook Miniflux Reader
A basic client to access an existing https://miniflux.app instance via a Pocketbook ebook-reader. 

<img src="/screenshots/minifluxScreen.bmp" width="15%" height="15%">&nbsp;&nbsp;<img src="/screenshots/ContentView.bmp" width="15%" height="15%">&nbsp;&nbsp;<img src="/screenshots/HnCommentView.bmp" width="15%" height="15%">&nbsp;&nbsp;<img src="/screenshots/mainMenu.bmp" width="15%" height="15%">&nbsp;&nbsp;<img src="/screenshots/contextMenu.bmp" width="15%" height="15%">

## Features

* Download articles and hackernews comments to read them later offline
* Show unread and starred items
* Open content in Pocketbook reader
* Display Hackernews comments
* Handle everything by touch or keys

## Installation
Download and unzip the file from releases and place the miniflux.app into the "applications" folder of your pocketbook. 
Get API Token from Settings -> API Keys and save it and the URL to the miniflux.cfg, which has to be placed in system->config->miniflux.
Once you disconnect the Pocketbook from the PC, the application should be visibile in the application launcher.


Furthermore in the Feed options "Fetch original content" has to be activated for each Feed.

## Usage

## Hackernews Comments
I use the RSS feed https://hnrss.org and set "Fetch original content" to yes. To read the comments a long click on the miniflux items opens a context menu where the comments can be opened. By downloaded the articles the loading times will decrease.

## How to build

First you need to install the basic build tools for linux.

Then you have to download the Pocketbook SDK (https://github.com/pocketbook/SDK_6.3.0/tree/5.19).

In the CMakeLists.txt of this project you have to set the root of the TOOLCHAIN_PATH to the location where you saved the SDK. 
This could be for example:

`SET (TOOLCHAIN_PATH "../../SDK/SDK_6.3.0/SDK-B288")`

Then you have to setup cmake by:

`cmake .`

To build the application run:

`make` 

## Disclamer
Use as your own risk! 
Even though the possibility is really low, the application could harm your device or even break it.
