import datetime

today = datetime.datetime.now()
doc_name = today.strftime("%y") + today.strftime("%m") + today.strftime("%d") + today.strftime("%H") + today.strftime("%M")

print(doc_name)