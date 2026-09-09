📺 IPTV for Samsung Tizen
<p align="center"> <strong>IPTV Player for Samsung Tizen TVs</strong> </p> <p align="center"> Xtream Codes • M3U • M3U8 • HLS • Remote Control </p> <p align="center"> <img src="https://img.shields.io/badge/platform-Samsung%20Tizen-00A4EF?style=flat-square" alt="Samsung Tizen"> </p>

A lightweight IPTV player designed specifically for Samsung Tizen TVs.

The application runs as a Tizen Web Application and supports Xtream Codes, M3U/M3U8 playlists, HLS streams, channel categories, multiple IPTV profiles, remote control navigation, and more.

✨ Features
📺 Samsung Tizen TV support
🔌 Xtream Codes support
📋 M3U / M3U8 playlist support
▶️ HLS and TS stream support
👤 Multiple IPTV profiles
📂 Channel categories
🔎 Channel search
🔢 Quick channel switching by number
💾 Last watched channel memory
🖼️ Channel logo support
⚡ Optimized rendering for large channel lists
🎮 Samsung TV remote control support
🔊 Volume control
🔇 Mute support
⛶ Full-screen playback
💡 Philips Hue synchronization
📺 Platform

This project is built specifically for Samsung Tizen TVs.

The application runs as a Tizen Web Application directly on the TV.

Requirements

To run the application, you need:

Samsung Tizen TV
Tizen Studio
Samsung TV Extension
A computer and Samsung TV connected to the same local network
🛠️ Installation
1. Install Tizen Studio

Install Tizen Studio and make sure the Samsung TV Extension is installed.

In Tizen Studio, open:

Tools
→ Package Manager


Install the required Samsung TV development tools and extensions.

2. Enable Developer Mode on the TV

On your Samsung TV, open:

Apps
→ Developer Mode
→ Developer Mode: ON


Enable Developer Mode.

Enter your computer's IP address on the Developer Mode screen and restart the TV.

The computer and TV must be connected to the same local network.

3. Connect the TV to Tizen Studio

Open:

Tools
→ Device Manager


Add your Samsung TV and establish the connection.

Once connected successfully, the TV should appear as an available device.

▶️ Running the Application

Import the project into Tizen Studio.

Then select:

Run As
→ Tizen Web Application


Tizen Studio will install and launch the application on the connected Samsung TV.

📦 Building a WGT Package

To create an installable Tizen package:

Project
→ Build Signed Package


After a successful build, a .wgt package will be generated.

Example:

IPTV.wgt

🔐 Tizen Certificate

A certificate profile is required to sign a Tizen application.

Open:

Tools
→ Certificate Manager


For development purposes, you can create a Samsung TV certificate profile.

For Samsung TV Store distribution, you must follow Samsung's current certificate and application-signing requirements.

🏪 Samsung TV Store

Publishing the application to the Samsung TV Store requires a separate submission and review process.

The general workflow is:

Developer Account
        ↓
Seller / Partner Registration
        ↓
Application Preparation
        ↓
Signed WGT
        ↓
TV Testing
        ↓
Store Submission
        ↓
Samsung Review
        ↓
Approval
        ↓
Store Publication


Depending on Samsung's current requirements, the submission may require:

Application name
Application description
Application icon
TV screenshots
Category
Age rating
Privacy policy
Support information
Signed WGT package

Samsung's Store requirements may change over time. Always check the latest Samsung Developer documentation before submitting an application.

👤 IPTV Profiles

The application supports multiple IPTV profiles.

Supported Sources
XTREAM
M3U8

Each profile can be configured and used independently.

🔌 Xtream Codes

An Xtream profile requires:

Server
Username
Password


Example:

Server:
https://example.com

Username:
user

Password:
password

Live Categories
/player_api.php?action=get_live_categories

Live Channels
/player_api.php?action=get_live_streams

Stream URL
/live/{username}/{password}/{stream_id}.ts

📋 M3U / M3U8

The application can load playlists using an M3U or M3U8 URL.

M3U Playlist
https://example.com/playlist.m3u

M3U8 Playlist
https://example.com/playlist.m3u8

Direct HLS Stream
https://example.com/live/channel.m3u8

Supported M3U Attributes

The application supports common playlist attributes such as:

tvg-logo
group-title

Example:

#EXTM3U

#EXTINF:-1 tvg-logo="https://example.com/logo.png" group-title="News",Example News
https://example.com/live/news.m3u8

#EXTINF:-1 tvg-logo="https://example.com/logo.png" group-title="Sports",Example Sports
https://example.com/live/sports.m3u8

🎮 Remote Control

The application is designed for navigation using the standard Samsung TV remote.

Button	Action
↑	Navigate up
↓	Navigate down
←	Navigate left
→	Navigate right
OK	Select channel
OK	Enter full-screen playback
BACK	Exit full screen
0–9	Enter channel number
Volume	Adjust volume
Mute	Mute audio
🔢 Channel Number Navigation

Use the numeric buttons on the remote to quickly select a channel.

For example:

1 → 2


enters channel number:

12


If channel 12 exists, it will be selected automatically.

💾 Last Watched Channel

The application remembers the last watched channel for the active IPTV profile.

When the application is launched again, the previously watched channel can be restored.

Each IPTV profile stores its own last-watched channel independently.

⚡ Performance

The channel list is optimized for large IPTV playlists.

Instead of rendering every channel at once, the application can limit rendering to the rows currently required on screen.

This helps provide:

Lower memory usage
Faster list rendering
Smoother scrolling
Better performance with large playlists
🌈 Philips Hue

The application can synchronize Philips Hue lighting with the currently playing video.

The general workflow is:

Video
  ↓
Color Analysis
  ↓
Hue Sync
  ↓
Ambient Lighting

🌐 Network & Compatibility

The IPTV provider's server must be accessible from the Samsung Tizen TV.

If a stream does not load, check the following:

Internet connection
IPTV server availability
HTTPS certificate
HTTP / HTTPS compatibility
CORS configuration
DNS resolution
Video codec support
Stream format compatibility
Samsung Tizen TV model compatibility
🎥 Supported Stream Formats

The application is dependent on the formats provided by the IPTV service.

Targeted formats include:

M3U
M3U8
HLS
TS

Codec and playback support may vary depending on the Samsung Tizen TV model and firmware version.

🔒 Security

Never hard-code IPTV credentials into the source code.

If this repository is public, do not commit sensitive information, including:

Username
Password
API Key
IPTV Credentials
Access Tokens


Use configuration files, environment variables, or secure storage where appropriate.

⚠️ Legal Notice

This project does not provide IPTV streams, channel lists, subscriptions, or copyrighted content.

Users are responsible for ensuring that they have the legal right to access and use any IPTV service or content configured in the application.

The project and its developers are not responsible for content, streams, or services provided by third-party IPTV providers.

🤝 Contributing

Contributions are welcome!

To contribute:

Fork the repository.
Create a feature branch.
Make your changes.
Commit your changes.
Push the branch.
Open a Pull Request.

Example:

git checkout -b feature/my-feature

git add .

git commit -m "Add my feature"

git push origin feature/my-feature


Then open a Pull Request on GitHub.

🐛 Bug Reports & Feature Requests

Found a bug or have an idea for a new feature?

Please use GitHub Issues to report problems or submit feature requests.

{"fallbackMarkdown":"GitHub Issues","reference":{"matched_text":"","prefix":null,"start_idx":8629,"end_idx":8688,"safe_urls":[],"refs":[],"alt":"GitHub Issues","prompt_text":"GitHub Issues","type":"url","title":"GitHub Issues","item":{"title":"GitHub Issues","url":"https://github.com/mobilkod/iptv/issues?utm_source=chatgpt.com","attribution":"github.com","pub_date":null,"snippet":null,"attribution_segments":null,"supporting_websites":null,"refs":[],"hue":null,"attributions":null},"layout":null,"logo":null},"showLoginRequiredCard":false}

⭐ Repository

{"fallbackMarkdown":"IPTV GitHub Repository","reference":{"matched_text":"","prefix":null,"start_idx":8711,"end_idx":8772,"safe_urls":[],"refs":[],"alt":"IPTV GitHub Repository","prompt_text":"IPTV GitHub Repository","type":"url","logo":null,"layout":null,"item":{"title":"IPTV GitHub Repository","url":"https://github.com/mobilkod/iptv?utm_source=chatgpt.com","attribution":"github.com","pub_date":null,"snippet":null,"attribution_segments":null,"supporting_websites":null,"refs":[],"hue":null,"attributions":null},"title":"IPTV GitHub Repository"},"showLoginRequiredCard":false}

<p align="center"> Made for Samsung Tizen TV 📺 </p>