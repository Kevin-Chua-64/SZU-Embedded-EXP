import socket
import time
import tkinter as tk
import threading
import queue

def TCP():
    serverPort = 9999
    serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serverSocket.bind(('', serverPort))
    serverSocket.listen(1)
    while True:
        print('The server is ready.')
        connectionSocket, addr = serverSocket.accept()
        print('Connected!')
        try:
            # 先发送时间
            request = connectionSocket.recv(1024).decode()
            print('rec:', request)
            cur_time = time.localtime(time.time())
            hour = time.strftime("%H", cur_time)
            min = time.strftime("%M", cur_time)
            sec = time.strftime("%S", cur_time)
            response = 'E1:' + hour + ',E2:' + min + ',E3:' + sec + ' \r\n'
            print('send:', response)
            connectionSocket.send(response.encode('gbk'))
            # 以后进行数据收发
            while True:
                request = connectionSocket.recv(1024).decode()
                print('rec:', request)
                if q_info.full():  # 队列满了就舍弃之前的
                    q_info.get()
                q_info.put(request)
                    
                if not q_key.empty():  # 队列非空就从中取键值
                    response = q_key.get()
                    print('send:', response)
                else:
                    response = "NULL"
                connectionSocket.send(response.encode('gbk'))
        except:
            print('Connection close.')


class UI(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title('UI')
        self.geometry('700x400')
        self.setup()

    def setup(self):
        # 显示信息的画布
        canvas_1 = tk.Frame(self, height=200, width=500)
        canvas_1.pack(side=tk.TOP)
        title_1 = tk.Label(canvas_1, text='Information', font=30)
        title_1.place(x=160, y=15, width=180, height=40)
        self.info_zone = tk.Text(canvas_1, relief='ridge', wrap='none')
        self.info_zone.place(x=10, y=60, width=470, height=120)
        self.info_zone.configure(state='disabled')

        # 按键区
        key = tk.Frame(self, height=200, width=450)
        key.pack(side=tk.TOP)
        title_2 = tk.Label(key, text='Key', font=30)
        title_2.place(x=135, y=15, width=180, height=40)

        # 按键
        btn1 = tk.Button(key, text='1', bg='light blue', command=self.key_1)
        btn1.place(y=70, x=0, width=100, height=40)

        btn2 = tk.Button(key, text='2', bg='light blue', command=self.key_2)
        btn2.place(y=70, x=175, width=100, height=40)

        btn3 = tk.Button(key, text='3', bg='light blue', command=self.key_3)
        btn3.place(y=70, x=350, width=100, height=40)

        btn4 = tk.Button(key, text='4', bg='light blue', command=self.key_4)
        btn4.place(y=140, x=0, width=100, height=40)

        btn5 = tk.Button(key, text='5', bg='light blue', command=self.key_5)
        btn5.place(y=140, x=175, width=100, height=40)

        btn6 = tk.Button(key, text='6', bg='light blue', command=self.key_6)
        btn6.place(y=140, x=350, width=100, height=40)

    # 按下按键执行功能，将键值入队
    def key_1(self):
        if q_key.full():
            q_key.get()
        q_key.put("K1\r\n")

    def key_2(self):
        if q_key.full():
            q_key.get()
        q_key.put("K2\r\n")

    def key_3(self):
        if q_key.full():
            q_key.get()
        q_key.put("K3\r\n")

    def key_4(self):
        if q_key.full():
            q_key.get()
        q_key.put("K4\r\n")

    def key_5(self):
        if q_key.full():
            q_key.get()
        q_key.put("K5\r\n")

    def key_6(self):
        if q_key.full():
            q_key.get()
        q_key.put("K6\r\n")

    # 定时1s执行函数，更新信息并显示
    def info_update(self):
        if not q_info.empty():  # 队列非空就从中取信息
            info = q_info.get()
            # 解析
            data = info.split(',')
            fan_mode = data[0]
            rotate_speed = data[1]
            fan_speed = data[2]
            timer_mode = data[3]
            timer_min_H = data[4]
            timer_min_L = data[5]
            timer_sec_H = data[6]
            timer_sec_L = data[7]
            # 显示
            self.info_zone.configure(state='normal')
            self.info_zone.delete('1.0', tk.END)
            self.info_zone.insert('1.0', '  Fan Mode:     %s\r\n  Rotate Speed: %s\r\n  Fan Mode:     %s\r\n  Timer Mode:   %s\r\n  Timer Time:   %s%s\" %s%s\''%(fan_mode,
            rotate_speed, fan_speed, timer_mode, timer_min_H, timer_min_L, timer_sec_H, timer_sec_L))
            self.info_zone.configure(state='disabled')

        self.after(1000, self.info_update)  # 定时1s再执行


if __name__ == '__main__':
    q_info = queue.Queue(1)  # 接收队列
    q_key = queue.Queue(1)  # 发送队列
    
    def main_UI():
        ui = UI()
        ui.info_update()
        ui.mainloop()

    # 双线程使得TCP服务器和UI同时工作   
    thread1 = threading.Thread(target=TCP)
    thread2 = threading.Thread(target=main_UI)
    thread1.start()
    thread2.start()
