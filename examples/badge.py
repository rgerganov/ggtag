#!/usr/bin/env python3
import os
import sys
import json
import ggtag
import urllib.request
import argparse
import html.parser

# This is a hacky HTML parser to extract profile information from GitHub
class GHProfileParser(html.parser.HTMLParser):

    @staticmethod
    def has_itemprop(attrs, value):
        for attr in attrs:
            if attr[0] == 'itemprop' and attr[1] == value:
                return True
        return False

    @staticmethod
    def get_attr(attrs, key):
        for attr in attrs:
            if attr[0] == key:
                return attr[1]
        return None

    def __init__(self):
        super().__init__()
        self.profile = {}
        self.in_works_for = False
        self.in_homeloc = False
        self.in_url = False
        self.in_name = False
        self.in_followers = False

    def set_once(self, key, value):
        if key not in self.profile:
            self.profile[key] = value

    def handle_starttag(self, tag, attrs) -> None:
        if tag == 'li' and self.has_itemprop(attrs, 'worksFor'):
            self.in_works_for = True
        if tag == 'li' and self.has_itemprop(attrs, 'homeLocation'):
            self.in_homeloc = True
        if tag == 'li' and self.has_itemprop(attrs, 'url'):
            self.in_url = True
        if tag == 'span' and self.has_itemprop(attrs, 'name'):
            self.in_name = True
        if tag == 'span' and len(attrs) == 1 and attrs[0][0] == 'class' and attrs[0][1] == 'text-bold color-fg-default':
            self.in_followers = True
        if tag == 'span' and self.in_works_for:
            self.set_once('company', self.get_attr(attrs, 'title'))
        if tag == 'a' and self.in_url:
            self.set_once('blog', self.get_attr(attrs, 'href'))
        if tag == 'a' and self.has_itemprop(attrs, 'image'):
            self.set_once('avatar_url', self.get_attr(attrs, 'href'))
        return super().handle_starttag(tag, attrs)

    def handle_data(self, data: str) -> None:
        if self.in_name:
            self.set_once('name', data.strip())
        if self.in_homeloc and data.strip():
            self.set_once('location', data.strip())
        if self.in_followers:
            self.set_once('followers', data.strip())
        return super().handle_data(data)

def fetch_api_profile(token, user):
    url = "https://api.github.com/users/{}".format(user)
    headers = {
        'Accept': 'application/vnd.github+json',
        'Authorization': 'Bearer {}'.format(token),
        'X-GitHub-Api-Version': '2022-11-28'
    }
    req = urllib.request.Request(url, None, headers)
    response = urllib.request.urlopen(req).read()
    profile = json.loads(response)
    if profile['followers'] > 1000:
        profile['followers'] = '{}k'.format(profile['followers'] // 1000)
    else:
        profile['followers'] = '{}'.format(profile['followers'])
    return profile

def fetch_html_profile(user):
    url = "https://github.com/{}".format(user)
    response = urllib.request.urlopen(url)
    html_profile = response.read().decode('utf-8')
    parser = GHProfileParser()
    parser.feed(html_profile)
    return parser.profile

def tweak_profile(profile):
    if 'location' in profile:
        profile['location'] = profile['location'].replace(', ', ',')
    profile['followers'] = profile['followers'] + ' followers'
    return profile

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Creates a GitHub badge')
    parser.add_argument('username', type=str, help='GitHub username')
    parser.add_argument('-q', '--qrcode', action='store_true', help='Use QR code instead of profile image')
    args = parser.parse_args()
    username = args.username
    token = os.environ.get('GITHUB_TOKEN')
    if not token:
        print('GITHUB_TOKEN environment variable not set, using HTML parser')
        profile = fetch_html_profile(username)
    else:
        profile = fetch_api_profile(token, username)
    profile = tweak_profile(profile)
    profile_pic = profile['avatar_url']
    name = profile['name']
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

    placeholder_icons = [(13, 156), (13, 183), (180, 154), (180, 185)]
    placeholder_texts = [(33, 158), (33, 185), (202, 158), (202, 185)]
    icon_map = {'location': 'map-marker-alt',
                'company': 'building',
                'blog': 'link',
                'email': 'envelope',
                'followers': 'user-friends'}
    props = ['location', 'company', 'blog', 'followers', 'email']
    index = 0
    for prop in props:
        if profile.get(prop) and index < 4:
            tag.icon(*placeholder_icons[index], 16, icon_map[prop])
            tag.text(*placeholder_texts[index], 2, profile[prop])
            index += 1
    tag.browse()
