# -*- coding: UTF-8 -*-   
import numpy
import sys
from random import random, randint
#from pylab import *
from datetime import datetime, timedelta, date

import linklist

reload(sys)
sys.setdefaultencoding('utf-8')

# 网格总长度
Define_Size = 1998

# 测试模式
Test_Moudle = ' light' #灯塔 'Nine' 九宫格 'link' 十字链表 'all' 全场景同步


# 定义一个含有障碍物的20×20的栅格地图
# 10表示可进入的格子
# 0表示其他对象
# 5表示观察对象
print("size = ",Define_Size)
map_grid = numpy.full((Define_Size, Define_Size), int(10), dtype=numpy.int8)

def calc_index(center_x, center_y):
    return int(center_x << 8) + int(center_y)

def calc_map_index_by_coordinate(x, y):
    c_x = (3 * (x // 3)) + 1
    c_y = (3 * (y // 3)) + 1
    return calc_index(c_x, c_y)

def calc_neighbor_map_index(x, y):
    result = set()
    x_arr = [x-3, x, x+3]
    y_arr = [y-3, y, y+3]
    for x_a in x_arr:
        if x_a < 0 or x_a > Define_Size:
            continue
        for y_a in y_arr:
            if y_a < 0 or y_a > Define_Size:
                continue
            result.add(calc_index(x_a, y_a))
    
    return result

def create_map_in_begin():
    index_x = 1
    index_y = 1
    while index_x < Define_Size:
        index_y = 1
        while index_y < Define_Size:
            map_new = Map(index_x, index_y)
            map_manger.add_map(map_new)
            #print('man_size={},map_manger.map_mgr={}'.format(sys.getsizeof(map_new),sys.getsizeof(map_manger.map_mgr)))
            map_grid[index_x, index_y] = 8
            index_y += 3
        index_x += 3

## 记录发送的消息总字节数
all_send_msg_size = 0

# 地图
class Map(object):
    """
    地图
    """
    def __init__(self, x, y):
        self.center_x = x
        self.center_y = y
        self.index = calc_index(self.center_x, self.center_y)
        self.neighbor_map_index = calc_neighbor_map_index(self.center_x, self.center_y)

        self.act_in_area = set() # 在区域内的真实对象
        
        self.sub_act = set()## 灯塔算法中的观察列表
        


    def add_act(self, act):
        self.act_in_area.add(act)
        #print("add_act",self.act_in_area)

    def add_sub_act(self, act):
        self.sub_act.add(act)
        self.send_map_msg_to_new(act)

    def del_act(self, act):
        self.act_in_area.remove(act)

    def del_sub_act(self, act):
        self.sub_act.remove(act)
        self.send_leave_msg_to_other(act)
        self.send_map_msg_to_new(act)

    # 普通移动和第一次生成时的数据发送
    def send_move_msg_in_normal(self, act, log):
        for sub in self.sub_act:
            if act != sub:
                global all_send_msg_size
                all_send_msg_size += 10
                print("to_other + " + log)
    
    # act跨map时没有变化的map发送移动信息
    def send_move_msg_in_change(self, act):
        for r_act in self.act_in_area:
            if r_act !=  act:
                global all_send_msg_size
                all_send_msg_size += 10
                print("send_move_msg_in_change")

    # 发送map上的real_act的数据给act(包括进入视野和离开视野的map)
    def send_map_msg_to_new(self, act):
        all_size = len(self.act_in_area)
        if self.index == act.map_index:
            all_size = max((all_size - 1),0)
        global all_send_msg_size
        all_send_msg_size += 10 * all_size
        print("to_new", all_size)
        
    # act离开视野发送给离开的others
    def send_leave_msg_to_other(self, act):
        for act_in in self.act_in_area:
            if act != act_in:
                global all_send_msg_size
                all_send_msg_size += 10
                print("leave map")

        
class Map_Mgr(object):
    """
    地图管理
    """
    def __init__(self):
        self.map_mgr = {}
        self.map_act = set()
        self.LinkedList_X = linklist.LinkedListTwoway()
        self.LinkedList_Y = linklist.LinkedListTwoway()

    def add_map(self, t_map):
        self.map_mgr[t_map.index] = t_map

    def get_map_by_index(self ,index):
        if index not in self.map_mgr:
            return False, ""
        else:
            return True,self.map_mgr[index]
    
    # 地图切换
    def calc_change_sub_act_map(self ,old_map, now_map, act):
        if not old_map and not now_map:
            return False
        
        if Test_Moudle == 'light':
            now_map.send_move_msg_in_normal(act, 'change map')

        if not old_map: ## act第一次加入
            for now_neighbor_index in now_map.neighbor_map_index:
                now_result, now_neighbor_map = self.get_map_by_index(now_neighbor_index)
                if not now_result:
                    print("get_map_by_index now error " + str(now_neighbor_index))
                    return False
                if Test_Moudle == 'Nine':
                    for v_act in now_neighbor_map.act_in_area:
                        if v_act == act:
                            continue
                        v_act.send_entry_msg()
                        act.viewcomponent.add_view(v_act)
                    now_neighbor_map.send_map_msg_to_new(act)
                elif Test_Moudle == 'light':
                    now_neighbor_map.add_sub_act(act)
            return True

        for old_neighbor_index in old_map.neighbor_map_index:
            old_result,old_neighbor_map = self.get_map_by_index(old_neighbor_index)
            if not old_result:
                print("get_map_by_index old error " + old_neighbor_index)
                return False
            if old_neighbor_index not in now_map.neighbor_map_index:
                if Test_Moudle == 'Nine':
                    for v_act in old_neighbor_map.act_in_area:
                        v_act.send_leave_msg()
                        act.viewcomponent.del_view(v_act)
                    old_neighbor_map.send_map_msg_to_new(act)
                elif Test_Moudle == 'light':
                    old_neighbor_map.del_sub_act(act)

        if Test_Moudle == 'Nine':
            act.send_move_msg()
        
        for now_neighbor_index in now_map.neighbor_map_index:
            if now_neighbor_index not in old_map.neighbor_map_index:
                now_result, now_neighbor_map = self.get_map_by_index(now_neighbor_index)
                if not now_result:
                    print("get_map_by_index now error " + now_neighbor_index)
                    return False
                if Test_Moudle == 'Nine':
                    for v_act in now_neighbor_map.act_in_area:
                        v_act.send_entry_msg()
                        act.viewcomponent.add_view(v_act)
                    now_neighbor_map.send_map_msg_to_new(act)
                elif Test_Moudle == 'light':
                    now_neighbor_map.add_sub_act(act)

        return True

    def act_add_in_link(self, act, x_ret, y_ret):
        if act not in self.map_act:
            self.map_act.add(act)
        #print("map-act size = {}, x = {}, y={}".format(len(self.map_act),act.x, act.y))
        if x_ret:
            act.x_node = self.LinkedList_X.add_by_sort(act, 'x')
        if y_ret:
            act.y_node = self.LinkedList_Y.add_by_sort(act, 'y')

        '''
        print(" add act ", len(self.LinkedList_X), len(self.LinkedList_Y))
        if act.x_node.prev is not None:
            act.x_node.prev.value.x_node.next = act.x_node
        if act.x_node.next is not None:
            act.x_node.next.value.x_node.perv = act.x_node

        if act.y_node.prev is not None:
            act.y_node.prev.value.y_node.next = act.y_node
        if act.y_node.next is not None:
            act.y_node.next.value.y_node.perv = act.y_node
        '''
        
        #print(" after add act ", len(self.LinkedList_X), len(self.LinkedList_Y))
        #print("id = {}, self = {}, prev = {}, next = {},selfy = {},yprev = {}, ynext = {}".format(act.id, act.x_node, act.x_node.prev, act.x_node.next, act.y_node, act.y_node.prev, act.y_node.next))
        

    def act_move_before_in_link(self, act, x_ret, y_ret):
        '''
        if act.x_node.prev is not None:
            act.x_node.prev.value.x_node.next = act.x_node.next
        if act.x_node.next is not None:
            act.x_node.next.value.x_node.perv = act.x_node.prev

        if act.y_node.prev is not None:
            act.y_node.prev.value.y_node.next = act.y_node.next
        if act.y_node.next is not None:
            act.y_node.next.value.y_node.perv = act.y_node.prev
        '''

        #print("delete act ", act, len(self.LinkedList_X), len(self.LinkedList_Y))
        if x_ret:
            self.LinkedList_X.remove(act)
        if y_ret:
            self.LinkedList_Y.remove(act)
        #print("after delete act ", len(self.LinkedList_X), len(self.LinkedList_Y))

    def act_add_in_all(self, act):
        if act not in self.map_act:
            self.map_act.add(act)





map_manger = Map_Mgr()
create_map_in_begin()