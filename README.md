<p align="center">
  <img src="logo.png" />
</p>


# Ez_voder



A Qt-based desktop application for downloading Twitch clips/VODs and uploading them directly to YouTube.

## Features

- Modern, animated Qt-based user interface
- Download Twitch clips and VODs with quality selection
- Upload downloaded videos to YouTube
- Progress tracking for both downloads and uploads
- Support for public/private video visibility on YouTube

## Requirements

- Qt6
- Python 3.x
- CMake 3.16 or higher
- C++17 compatible compiler

### Python Dependencies
```
google-auth-oauthlib
google-api-python-client
```

## Building from Source

1. Clone the repository:
```bash
git clone https://github.com/Tomieka/Ez_voder.git
cd Ez_voder
```

2. Install Python dependencies:
```bash
pip install -r requirements.txt
```

3. Create a build directory and build the project:
```bash
mkdir build
cd build
cmake ..
make
```

## Setup

1. Create a project in the Google Cloud Console
2. Enable the YouTube Data API v3
3. Create OAuth 2.0 credentials
4. Download the credentials and save them as `client_secrets.json` in the same directory as the executable

## Usage

1. Launch the application
2. For downloading Twitch content:
   - Enter the Twitch clip/VOD URL
   - Select desired quality
   - Click "Download"

3. For uploading to YouTube:
   - Enter video title and description
   - Choose visibility (public/private)
   - Click "Upload to YouTube"
   - Follow the authentication process on first use

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request 
