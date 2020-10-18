# -*- coding: UTF-8 -*-   
import numpy
import sys
from random import random, randint
from pylab import *
from datetime import datetime, timedelta, date
import time
import inspect
import objgraph

import actor
import map

reload(sys)
sys.setdefaultencoding('utf-8')

leader_path = [(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(1,0),(1,0),(1,0),(1,0),(1,0),(0,-1),(0,-1),(0,-1),(0,-1),(0,-1),(0,-1),(0,-1),(0,-1),(0,-1),(1,0)]

def rand_create_other_auctor(auctor_num):
    while auctor_num > 0:
        x = randint(0,map.Define_Size)
        y = randint(0,map.Define_Size)
        print('x={},y={}'.format(x,y))
        act = actor.Auctor(auctor_num, x, y)
        auctor_num -= 1
        
def rand_move_other_auctor(now_time):
    for map_one in map.map_manger.map_mgr.items():
        t_map = map_one[1]
        for act in t_map.act_in_area.copy():
            act.rand_move(now_time)
                

# 十字链表随机移动
def rand_move_link_actor(now_time):
    for act in map.map_manger.map_act:
        act.rand_move(now_time)


        
def draw_all_act():
    plt.ion()
    
    plt.imshow(map.map_grid, cmap=plt.cm.hot, interpolation='nearest', vmin=0, vmax=10)
    plt.colorbar()
    xlim(-1, map.Define_Size)  # 设置x轴范围
    ylim(-1, map.Define_Size)  # 设置y轴范围
    my_x_ticks = numpy.arange(0, map.Define_Size, 1)
    my_y_ticks = numpy.arange(0, map.Define_Size, 1)
    plt.xticks(my_x_ticks)
    plt.yticks(my_y_ticks)
    plt.grid(b=True,which='major',axis='both',alpha= 0.5,color='skyblue',linestyle='--',linewidth=2)
    plt.pause(1)
    #input("please enter ")
    plt.clf()


def get_size(obj, seen=None):
    size = sys.getsizeof(obj)
    if seen is None:
        seen = set()
    obj_id = id(obj)
    if obj_id in seen:
        return 0
    seen.add(obj_id)
    if hasattr(obj, '__dict__'):
        for cls in obj.__class__.__mro__:
            if '__dict__' in cls.__dict__:
                d = cls.__dict__['__dict__']
                if inspect.isgetsetdescriptor(d) or inspect.ismemberdescriptor(d):
                    size += get_size(obj.__dict__, seen)
                break
    if isinstance(obj, dict):
        # 这里避免重复计算
        size += sum((get_size(v, seen) for v in obj.values() if not isinstance(v, (str, int, float, bytes, bytearray))))
        # size += sum((get_size(k, seen) for k in obj.keys())) 
    elif hasattr(obj, '__iter__') and not isinstance(obj, (str, bytes, bytearray)):
        # 这里避免重复计算
        size += sum((get_size(i, seen) for i in obj if not isinstance(i, (str, int, float, bytes, bytearray))))

    if hasattr(obj, '__slots__'): 
        size += sum(get_size(getattr(obj, s), seen) for s in obj.__slots__ if hasattr(obj, s))

    return size

if __name__ == "__main__":
    
    map.Test_Moudle = sys.argv[1]
    act_num = sys.argv[2]
    map.Define_Size = int(sys.argv[3])
    #print("112",map.Define_Size)
    lead_act = actor.Auctor(10000, map.Define_Size/2, 0 , 1)#map.Define_Size/2, 1)
    #actor.Auctor(10001, map.Define_Size/2, 8 , 0)
    rand_create_other_auctor(int(act_num))
    #objgraph.show_refs([lead_act], filename='ref_topo.png')
    
    #test_set = set()
    #map_new = map.Map(1, 2)
    #print("11111",get_size(lead_act), get_size(lead_act.id), get_size(lead_act.x), get_size(lead_act.y), get_size(lead_act.c), 
    #get_size(lead_act.tick_time), get_size(lead_act.x_node), get_size(lead_act.y_node), get_size(lead_act.viewcomponent), get_size(lead_act.map_index), get_size(lead_act.old_map))
    #print("mem size:map_grid={},manger_size={},map_size={},act_size={}".format(get_size(map.map_grid), get_size(map.map_manger), get_size(map.map_manger.map_mgr), get_size(map.map_manger.map_act)))
    now_time = 0
    while True:
    #for path in leader_path:
        now_time += 1 #time.time()
        print("all_send_msg_size :" + str(map.all_send_msg_size))

        if map.Test_Moudle == 'link' or map.Test_Moudle == 'all':
            rand_move_link_actor(now_time)
        else:
            rand_move_other_auctor(now_time)
        
        draw_all_act()
        #print("Test_Moudle",map.Test_Moudle)
    
