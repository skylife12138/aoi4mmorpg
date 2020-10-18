# -*- coding: UTF-8 -*-   
import numpy
import sys
from random import random, randint
#from pylab import *
from datetime import datetime, timedelta, date


import map
import linklist

reload(sys)
sys.setdefaultencoding('utf-8')

# 视野内最大actor数量
max_view_actor_num = 10 

# 链表最大视野范围
max_link_view_range = 4

class ViewComponent(object):
    def __init__(self, act):
        self.view_list = []
        self.count = 0
        self.act = act

    def isfull(self):
        if len(self.view_list) >= max_view_actor_num:
            return True
        return False
    
    def is_in_view(self, v_act):
        for t_act in self.view_list:
            if t_act[0] == v_act:
                return True 
        return False

    def add_view(self, v_act):
        if self.isfull() or v_act.viewcomponent.isfull():
            return
        if self.act == v_act:
            return

        v_act.viewcomponent.view_list.append((self.act, self.count))
        self.view_list.append((v_act, v_act.viewcomponent.count))
        self.count += 1
        v_act.viewcomponent.count += 1

    def del_view(self, v_act):
        t_index = -1
        for t_act in self.view_list:
            if t_act[0] == v_act:
                t_index = t_act[1]
                break
        if t_index == -1:
            return
        
        if t_index >= v_act.viewcomponent.count or v_act.viewcomponent.view_list[t_index][0] != self.act:
            print("error viewlist index", t_index, v_act.viewcomponent.count, v_act.viewcomponent.view_list[t_index][0], self.act)
            return

        self.del_view_by_index(v_act.viewcomponent.view_list[t_index][1])        
        v_act.viewcomponent.del_view_by_index(t_index)


    def del_view_by_index(self, v_index):
        self.count -= 1
        if self.count == v_index:
            self.view_list.pop(v_index)
            return
        self.view_list[v_index] = self.view_list[self.count]
        self.view_list.pop(self.count)

        f_act = self.view_list[v_index][0]
        f_index = self.view_list[v_index][1]

        f_act.viewcomponent.view_list[f_index] = (self.act, v_index)


class Auctor(object):
    """
    可以移动的对象
    """
    def __init__(self, id, x, y, color = 0):
        self.id = id
        self.x = x
        self.y = y
        self.c = color
        self.tick_time = 0
        self.set_act_grid()
        self.edgearr = [0,0,0,0]
        #print("id:",self.id, id(self.id), self.x, id(self.x), self.y, id(self.y), self.c, id(self.c), self.tick_time, id(self.tick_time))
        if map.Test_Moudle == 'all':
            map.map_manger.act_add_in_all(self)
            self.send_all_msg()
            return

        self.x_node = None 
        self.y_node = None 
        if map.Test_Moudle == 'link':
            map.map_manger.act_add_in_link(self, True, True)
            self.send_link_msg()
            return

        self.viewcomponent = ViewComponent(self) #九宫格视野列表
        self.map_index = map.calc_map_index_by_coordinate(self.x, self.y)
        now_result, now_map = map.map_manger.get_map_by_index(self.map_index)
        if not now_result:
            print("auctor error")
            return
        self.old_map =  now_map 
        now_map.add_act(self)
        result = map.map_manger.calc_change_sub_act_map(None, now_map, self)
        if not result:
            print("calc_change_sub_act_map error")
    
    def __eq__(self, other):
        return self.id == other.id
        
    def set_edge_arr(self, tick_time):
        if self.x == 0:
            self.edgearr[0] = tick_time
        elif self.x == map.Define_Size - 1:
            self.edgearr[1] = tick_time

        if self.y == 0:
            self.edgearr[2] = tick_time
        elif self.y == map.Define_Size - 1:
            self.edgearr[3] = tick_time
        
    def move_to(self, to_x, to_y, tick_time):
        
        if to_x == 0 and to_y == 0:
            return 
        if self.tick_time == tick_time:#防止换灯塔时在一帧中移动两次
            return
        
        self.tick_time = tick_time
        self.clear_act_grid_color()

        self.x = min(map.Define_Size-1, max(0, self.x + to_x))
        self.y = min(map.Define_Size-1, max(0, self.y + to_y))
        #print("change size", self.x, self.y)
        self.set_edge_arr(tick_time)
        self.set_act_grid()

        if map.Test_Moudle == 'all':
            self.send_all_msg()
            return

        if map.Test_Moudle == 'link':
            x_ret,y_ret = self.check_need_change_link()
            if not x_ret and not y_ret:
                self.send_link_msg()
                return
            map.map_manger.act_move_before_in_link(self, x_ret, y_ret)
            map.map_manger.act_add_in_link(self, x_ret,y_ret)
            self.send_link_msg()
            return

        now_map_index = map.calc_map_index_by_coordinate(self.x, self.y)
        if self.map_index != now_map_index:
            now_result, now_map = map.map_manger.get_map_by_index(now_map_index)
            if not now_result:
                print("map error result ", now_result)
                return
            self.old_map.del_act(self)
            now_map.add_act(self)
            result = map.map_manger.calc_change_sub_act_map(self.old_map, now_map, self)
            if not result:
                print("calc_change_sub_act_map error")
                return
            #print("change move",self.map_index,now_map_index)
            self.map_index = now_map_index
            self.old_map =  now_map
        else:
            if map.Test_Moudle == 'Nine':
                self.send_move_msg()
            elif map.Test_Moudle == 'light':
                self.old_map.send_move_msg_in_normal(self, "move normal")
        
        
    def set_act_grid(self):
        if map.map_grid[self.x, self.y] == 8:
            return
        if self.c == 0:
            map.map_grid[self.x, self.y] = 0
        elif self.c == 1:
            map.map_grid[self.x, self.y] = 5
            
    def clear_act_grid_color(self):
        if map.map_grid[self.x, self.y] == 8:
            return 
        map.map_grid[self.x, self.y] = 10
            
    def rand_move(self, tick_time):
        if randint(0, 1) == 0:
            x = -1
        else:
            x = 1
        
        if randint(0, 1) == 0:
            y  = -1
        else:
            y  = 1

        if self.edgearr[0] and self.edgearr[0] + randint(2,4) >= tick_time:
            x = 1
        elif self.edgearr[1] and self.edgearr[1] + randint(2,4) >= tick_time:
            x = -1

        if self.edgearr[2] and self.edgearr[2] + randint(2,4)  >= tick_time:
            y = 1
        elif self.edgearr[3] and self.edgearr[3] + randint(2,4)  >= tick_time:
            y = -1

        if x == 0 and y==0 and False:
            if self.x > map.Define_Size *2 /3:
                x = -1
            else:
                x = 1
            if self.y > map.Define_Size *2 /3:
                y = -1
            else:
                y = 1
        print("(x,y)=",x,y)
        self.move_to(x, y, tick_time)

    def send_entry_msg(self):
        print("send_entry_msg")
        map.all_send_msg_size += 10

    def send_entry_msg_to_new(self):
        print("send_entry_msg_to_new")
        map.all_send_msg_size += 10

    def send_leave_msg(self):
        print("send_leave_msg")
        map.all_send_msg_size += 10

    def send_move_msg(self):
        if map.Test_Moudle == 'Nine':
            for v_act in self.viewcomponent.view_list:
                print("send_move_msg")
                map.all_send_msg_size += 10

    def check_need_change_link(self):
        x_ret = False
        y_ret = False
        if (self.x_node.prev and (self.x < self.x_node.prev.value.x)) or (self.x_node.next and (self.x > self.x_node.next.value.x)):
            x_ret = True
        if (self.y_node.prev and (self.y < self.y_node.prev.value.y)) or (self.y_node.next and (self.y > self.y_node.next.value.y)):
            y_ret = True
        return x_ret,y_ret

    def get_link_view_data(self):
        x_data = set()
        y_data = set()
        xl_node = self.x_node.prev
        while  xl_node is not None:
            #print("x1 = {}, x2={}".format(self.x_node.value.id , xl_node.value.id))
            if self.x_node.value.x - xl_node.value.x > max_link_view_range:
                break
            x_data.add(xl_node.value)
            xl_node = xl_node.prev

        xl_node = self.x_node.next
        while  xl_node is not None:
            #print("x11 = {}, x12={}".format(xl_node.value.id , self.x_node.value.id))
            if  xl_node.value.x - self.x_node.value.x > max_link_view_range:
                break
            x_data.add(xl_node.value)
            xl_node = xl_node.next

        yl_node = self.y_node.prev
        #print("yl_node 1", yl_node)
        while  yl_node is not None:
            #print("y1 = {}, y2={}".format(self.y_node.value.id , yl_node.value.id))
            if self.y_node.value.y - yl_node.value.y > max_link_view_range:
                break
            y_data.add(yl_node.value)
            yl_node = yl_node.prev

        yl_node = self.y_node.next
        #print("yl_node 2", yl_node)
        while  yl_node is not None:
            #print("y11 = {}, y12={}".format(yl_node.value.id ,self.y_node.value.id))
            if  yl_node.value.y - self.y_node.value.y > max_link_view_range:
                break
            y_data.add(yl_node.value)
            yl_node = yl_node.next

        return x_data.intersection(y_data)

    def send_link_msg(self):
        view_data = self.get_link_view_data()
        if len(view_data) == 0:
            return
        map.all_send_msg_size += 10
        for v_act in view_data:
            map.all_send_msg_size += 10
            print("send_link_msg")

    def send_all_msg(self):
        for v_act in map.map_manger.map_act:
            if v_act == self:
                continue
            map.all_send_msg_size += 10
