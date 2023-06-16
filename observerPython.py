



from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

class MyHandler(FileSystemEventHandler):
    def on_created(self, event):
        print(f'event type: {event.event_type}  path : {event.src_path}')

observer = Observer()
observer.schedule(MyHandler(), path='directory_to_watch', recursive=True)
observer.start()
