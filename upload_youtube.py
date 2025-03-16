#!/usr/bin/env python3
import os
import pickle
import sys
from google.oauth2.credentials import Credentials
from google_auth_oauthlib.flow import InstalledAppFlow
from google.auth.transport.requests import Request
from googleapiclient.discovery import build
from googleapiclient.http import MediaFileUpload
from googleapiclient.errors import HttpError

SCOPES = ['https://www.googleapis.com/auth/youtube.upload']

def get_authenticated_service():
    creds = None
    if os.path.exists('token.pickle'):
        with open('token.pickle', 'rb') as token:
            creds = pickle.load(token)
    
    if not creds or not creds.valid:
        if creds and creds.expired and creds.refresh_token:
            creds.refresh(Request())
        else:
            flow = InstalledAppFlow.from_client_secrets_file(
                'client_secrets.json', SCOPES)
            creds = flow.run_local_server(port=0)
        with open('token.pickle', 'wb') as token:
            pickle.dump(creds, token)
    
    return build('youtube', 'v3', credentials=creds)

def upload_video(youtube, file_path, title, description, privacy_status):
    body = {
        'snippet': {
            'title': title,
            'description': description,
            'tags': ['twitch', 'clip', 'gaming']
        },
        'status': {
            'privacyStatus': privacy_status,
            'selfDeclaredMadeForKids': False
        }
    }

    insert_request = youtube.videos().insert(
        part=','.join(body.keys()),
        body=body,
        media_body=MediaFileUpload(
            file_path, 
            chunksize=-1, 
            resumable=True
        )
    )

    response = None
    while response is None:
        try:
            status, response = insert_request.next_chunk()
            if status:
                print(f"Uploaded {int(status.progress() * 100)}%")
        except HttpError as e:
            print(f"An HTTP error {e.resp.status} occurred: {e.content}")
            return None
    
    return response

def main():
    if len(sys.argv) != 5:
        print("Usage: upload_youtube.py <file_path> <title> <description> <privacy>")
        return 1

    file_path = sys.argv[1]
    title = sys.argv[2]
    description = sys.argv[3]
    privacy = sys.argv[4]

    if not os.path.exists(file_path):
        print(f"Error: File not found: {file_path}")
        return 1

    if privacy not in ['private', 'unlisted', 'public']:
        print("Error: Privacy must be one of: private, unlisted, public")
        return 1

    try:
        youtube = get_authenticated_service()
        print(f"Starting upload of {file_path}...")
        
        response = upload_video(
            youtube,
            file_path,
            title,
            description,
            privacy
        )

        if response:
            print(f"Upload successful! Video ID: {response['id']}")
            print(f"Video URL: https://youtu.be/{response['id']}")
            return 0
        else:
            print("Upload failed.")
            return 1

    except HttpError as e:
        print(f"An HTTP error {e.resp.status} occurred: {e.content}")
        return 1
    except Exception as e:
        print(f"An error occurred: {e}")
        return 1

if __name__ == '__main__':
    exit(main())
