#!/usr/bin/env python3
import os
import requests
import pypresence
import time

from pypresence import ActivityType
from dotenv import load_dotenv

# WARNING WARNING WARNING
# this is probably janky as hell (i dont ever use python idk why i decided to use it for this)

def connect(client_id):
    while True:
        try:
            rpc = pypresence.Presence(client_id)
            rpc.connect()
            print("* connected to discord")
            return rpc
        except Exception as e:
            print(f"! failed to connect to discord: {e}")
            print("! will retry in 5 seconds...")
            time.sleep(5)
            continue

def fetch_metadata(title_id):
    # uses https://prosperopatches.com for PS5 games and apps and https://orbispatches.com for PS4 stuff
    # 
    # title id prefixes are from https://www.psdevwiki.com/ps5/Title_ID#Internal_Title_IDs, maybe should
    # also filter out NPXS (system reserved)
    if title_id.startswith(("ECAS", "ELAS", "ELJM", "PPSA")):
        # PS5 app or game, use prosperopatches
        url = f"https://prosperopatches.com/api/lookup?titleid={title_id}"
    else:
        # assume PS4 app or game instead, for which we use orbispatches
        url = f"https://orbispatches.com/api/lookup?titleid={title_id}"

    try:
        req = requests.get(url)
        req.raise_for_status()
        body = req.json()
    except (requests.RequestException, ValueError) as e:
        print(f"! failed to fetch metadata for {title_id}, {url} returned an error: {e}")
        return None

    if not body.get("success"):
        print(f"! failed to fetch metadata for {title_id}, {url} returned an error: {body}")
        return None
    
    return body.get("metadata")

def main():
    load_dotenv()

    client_id = os.getenv("CLIENT_ID")
    address = os.getenv("PS5_ADDRESS")
    port = os.getenv("PS5_PORT")

    if client_id is None:
        print("! CLIENT_ID must be set!")
        return

    if address is None or port is None:
        print("! PS5_ADDRESS and PS5_PORT must both be set!")
        return

    server = f"http://{os.getenv("PS5_ADDRESS")}:{os.getenv("PS5_PORT")}/"

    print(f"* ps5-discord-rpc - https://github.com/azurejelly/ps5-discord-rpc")
    print(f"* will fetch status from {server}")

    rpc = connect(client_id)
    last_sent = 0.0
    last_title_id = None
    fails = 0

    while True:
        try:
            req = requests.get(server, timeout=5)
            req.raise_for_status()
            body = req.json()
        except Exception as e: # FUCK IT WE BALL 2
            print(f"! failed to fetch status from {server}: {e}")
            fails += 1

            if fails >= 5 and last_title_id is not None:
                print("! failed to obtain status 5 times in a row, assuming PS5 has been turned off or something")
                last_title_id = None
                rpc.clear()

            time.sleep(1.5)
            continue

        title_id = body.get("title_id")
        should_force_refresh = title_id is not None and time.monotonic() - last_sent > 15

        # i dont know why it took me so long to come up with how to safely refresh the status in case the connection
        # broke or whatever (maybe i should've slept a bit in the last 36 hours). this just tries to update it every
        # 15 seconds and if it fails it reconnects
        if title_id != last_title_id or should_force_refresh:
            try:
                if title_id is None:
                    print("* no longer playing anything, clearing discord status...")
                    last_title_id = None
                    rpc.clear()
                else:
                    metadata = fetch_metadata(title_id)
                    if metadata:
                        title = metadata.get("name")
                        icon = metadata.get("icon")

                        if title_id != last_title_id:
                            print(f"* now playing: {title} - {title_id}")

                        rpc.update(
                            activity_type=ActivityType.PLAYING,
                            state=title_id,
                            details="Playing on PlayStation®5",
                            name=title,
                            large_image=icon or None, # some games have no icon (like balatro)
                            large_text=title
                        )

                        last_sent = time.monotonic()
                        last_title_id = title_id
                    else:
                        print(f"! the metadata for the current game ({title_id}) could not be fetched.")
                        print(f"! nothing will be displayed until the metadata is available!")
                        return
            except Exception as e: # fuck it we ball (im too tired for this shit)
                # should also try to update the status after reconnecting
                print(f"! discord connection lost, will try to reconnect: {e}")
                rpc = connect(client_id)

if __name__ == "__main__":
    main()