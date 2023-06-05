#!/usr/bin/env python3
import os
import sys
import json
import ggtag
import urllib.request
import argparse

def fetch_profile(token, user):
    url = "https://api.github.com/users/{}".format(user)
    headers = {
        'Accept': 'application/vnd.github+json',
        'Authorization': 'Bearer {}'.format(token),
        'X-GitHub-Api-Version': '2022-11-28'
    }
    req = urllib.request.Request(url, None, headers)
    response = urllib.request.urlopen(req).read()
    return json.loads(response)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Creates a GitHub badge')
    parser.add_argument('username', type=str, help='GitHub username')
    parser.add_argument('-q', '--qrcode', action='store_true', help='Use QR code instead of profile image')
    args = parser.parse_args()
    token = os.environ['GITHUB_TOKEN']
    if not token:
        print("Please set GITHUB_TOKEN environment variable")
        sys.exit(1)
    username = args.username
    profile = fetch_profile(token, username)
    profile_pic = profile['avatar_url']
    name = profile['name']
    location = profile['location']
    company = profile['company']
    blog = profile['blog']
    email = profile['email']
    if location and len(location) > 15:
        # try to make it shorter by removing some spaces
        location = location.replace(', ', ',')
    tag = ggtag.GGTag()
    tag.rect(10, 25, 110, 110)
    if not args.qrcode:
        tag.image_url(15, 30, 100, 100, True, profile_pic)
    else:
        if len(username) > 7:
            tag.qrcode(20, 35, 3, 'https://github.com/{}'.format(username))
        else:
            tag.qrcode(15, 30, 4, 'https://github.com/{}'.format(username))
    if len(name) > 12:
        first, second = name.split(' ', 1)
        tag.text(140, 40, 5, first)
        tag.text(140, 70, 5, second)
        tag.text(140, 110, 2, "github.com/{}".format(username))
    else:
        tag.text(140, 50, 5, name)
        tag.text(140, 80, 2, "github.com/{}".format(username))
    if location:
        tag.icon(13, 156, 16, 'map-marker-alt')
        tag.text(33, 158, 2, location)
    if company:
        tag.icon(13, 183, 16, 'building')
        tag.text(33, 185, 2, company)
    if blog:
        tag.icon(180, 154, 16, 'link')
        tag.text(202, 158, 2, blog)
    if email:
        tag.icon(180, 185, 16, 'envelope')
        tag.text(202, 185, 2, email)
    tag.browse()
