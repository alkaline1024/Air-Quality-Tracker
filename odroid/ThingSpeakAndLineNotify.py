#!/usr/bin/env python
import odroid_wiringpi as wpi
import time
import urllib
import requests
import datetime
from zoneinfo import ZoneInfo

quickReplyMessage = {
    "type": "text",
    "text": "เลือกตรวจสอบค่าฝุ่น",
    "quickReply": {
        "items": [
        {
            "type": "action",
            "action": {
            "type": "message",
            "label": "ค่าฝุ่นล่าสุด",
            "text": "ค่าฝุ่นล่าสุด"
            }
        },
        {
            "type": "action",
            "action": {
            "type": "message",
            "label": "ย้อนหลัง 1 ชั่วโมง",
            "text": "ย้อนหลัง 1 ชั่วโมง"
            }
        },
        {
            "type": "action",
            "action": {
            "type": "message",
            "label": "24 ชั่วโมง",
            "text": "24 ชั่วโมง"
            }
        },
        ]
    }
}
warning_level_message = {
    "type":"text",
    "text":"$ อยู่ในระดับที่ต้องเฝ้าระวัง\n\nควรเฝ้าระวังสุขภาพ ถ้ามีอาการเกี่ยวกับระบบทางเดินหายใจ ระคายเคืองตา ไม่ควรทำกิจกรรมกลางแจ้งนาน หรือใช้อุปกรณ์ป้องกัน ส่วนผู้ที่ต้องดูแลสุขภาพเป็นพิเศษ แล้วมีอาการทางสุขภาพ เช่น ไอ หายใจลำบาก ตาอักเสบ แน่นหน้าอก ปวดศีรษะ หัวใจเต้นไม่เป็นปกติ คลื่นไส้ อ่อนเพลีย ควรปรึกษาแพทย์",
    "emojis": [
        {
            "index": 0,
            "productId": "5ac21a18040ab15980c9b43e",
            "emojiId": "047",
        },
    ]
}
danger_level_message = {
    "type":"text",
    "text":"$ อยู่ในระดับอันตรายอย่างมาก\n\nควรหลีกเลี่ยงกิจกรรมกลางแจ้งทุกอย่างหลีกเลี่ยงพื้นที่ที่มีมลพิษทางอากาศสูง หรือใช้อุปกรณ์ป้องกันตนเองหากมีความจำเป็น หากมีอาการทางสุขภาพควรปรึกษาแพทย์",
    "emojis": [
        {
            "index": 0,
            "productId": "5ac21a18040ab15980c9b43e",
            "emojiId": "048",
        },
    ]
}

def handler_line_chatbot(pm1_0, pm2_5, pm10_0):
    try:
        if pm2_5 < 50:
            return
        elif pm2_5 < 100:
            level_message = warning_level_message
        else:
            level_message = danger_level_message

        access_token = 'vM/06TZq7lbmdS1xG0eopxNBkDcNNf0035dP95pNbgpvt31fvgyB7g86xkpDaRH6bYysY/v/OxgLYQwkqDF2/hdQuHHUHvdtPPOlsMvWqtIrO54CkLr2j2xx3MDE5z001+p6XJNQRNJOPgL704QAVgdB04t89/1O/w1cDnyilFU='
        headers = {
            'Content-Type': 'application/json',
            'Authorization': 'Bearer ' + access_token
        }
        
        user_ids = requests.get("https://4bad-35-229-44-141.ngrok-free.app/user_ids").json()['user_ids']
        print(user_ids)
        for userId in user_ids: 
            push_url = "https://api.line.me/v2/bot/message/push"
            current_time = datetime.datetime.now().astimezone(ZoneInfo('Asia/Bangkok')).strftime("%H:%M:%S %d %b %Y ")
            message_json = {
                "to": userId,
                "messages":[
                    {
                        "type":"text",
                        "text": f"$ ประกาศแจ้งเตือน\nค่าฝุ่นที่วัดได้เกินมาตรฐาน",
                        "emojis": [
                            {
                                "index": 0,
                                "productId": "5ac21a18040ab15980c9b43e",
                                "emojiId": "048",
                            },
                        ]
                    },
                    {
                        "type":"text",
                        "text": f"$ วัดล่าสุดเมื่อ\n{current_time}",
                        "emojis": [
                            {
                                "index": 0,
                                "productId": "5ac21a18040ab15980c9b43e",
                                "emojiId": "192",
                            },
                        ]
                    },
                    {
                        "type":"text",
                        "text": f"$ ค่าที่วัดได้\n\n\tPM1.0:  \t{pm1_0:.2f} µg/m3\n\tPM2.5:  \t{pm2_5:.2f} µg/m3\n\tPM10.0:\t{pm10_0:.2f} µg/m3",
                        "emojis": [
                            {
                                "index": 0,
                                "productId": "5ac21a18040ab15980c9b43e",
                                "emojiId": "129",
                            },
                        ]
                    },
                    level_message,
                    quickReplyMessage,
                ]
            }
            response = requests.post(push_url, headers=headers, json=message_json)
            print(response.text)
        return True
    except Exception as error:
        print("something went wrong: ", error)
        return False


serial = wpi.serialOpen('/dev/ttyS1', 19200)
wpi.wiringPiSetup()
baseURL = 'https://api.thingspeak.com/update?api_key=GXJMTTWAAFZZ6667'
data = {
    "pm1_0": [],
    "pm2_5": [],
    "pm10_0": [],
}
started_time = time.time()
while True:
    if wpi.serialDataAvail(serial):
        pms_data = ""
        received_char = wpi.serialGetchar(serial)
        if (received_char in [255, -1]):
            print("invalid value serialGetchar: ", received_char)
            continue

        while (chr(received_char) != "\n"):
            pms_data += chr(received_char)
            received_char = wpi.serialGetchar(serial)
        
        pm1_0, pm2_5, pm10_0 = pms_data.split(",")
        
        try:
            data['pm1_0'].append(int(pm1_0))
            data['pm2_5'].append(int(pm2_5))
            data['pm10_0'].append(int(pm10_0))
        except:
            print("error on parse int(pms_data)")
            continue
    
    ended_time = time.time()
    if ended_time - started_time > 15:
        started_time = time.time()
        if len(data['pm1_0']) == 0 or len(data['pm2_5']) == 0 or len(data['pm10_0']) == 0:
            print("error on trying to send request: PMS7003 data is empty")
            continue

        average_pm1_0 = sum(data['pm1_0']) / len(data['pm1_0'])
        average_pm2_5 = sum(data['pm2_5']) / len(data['pm2_5'])
        average_pm10_0 = sum(data['pm10_0']) / len(data['pm10_0'])

        data['pm1_0'] = []
        data['pm2_5'] = []
        data['pm10_0'] = []

        print("send request to: ", baseURL + f"&field1={average_pm1_0:.2f}&field2={average_pm2_5:.2f}&field3={average_pm10_0:.2f}")
        f = urllib.request.urlopen(baseURL + f"&field1={average_pm1_0:.2f}&field2={average_pm2_5:.2f}&field3={average_pm10_0:.2f}")
        f.read()
        f.close()

        handler_line_chatbot(average_pm1_0, average_pm2_5, average_pm10_0)

wpi.serialClose(serial)


