# -*- coding: UTF-8 -*-   
import numpy
import sys
from random import random, randint
#from pylab import *
from datetime import datetime, timedelta, date

reload(sys)
sys.setdefaultencoding('utf-8')

class Node(object):
    def __init__(self, value):
        self.value = value
        # 前驱区
        self.prev = None
        # 后继区
        self.next = None


class LinkedListTwoway(object):
    def __init__(self):
        self.__head = None

    def is_empty(self):
        return self.__head is None

    def __len__(self):
        count = 0
        cur = self.__head
        while cur:
            count += 1
            cur = cur.next
        return count

    def traverse(self):
        cur = self.__head
        while cur:
            print(cur.value)
            cur = cur.next

    def add(self, value):
        node = Node(value)
        if self.is_empty():
            self.__head = node
        else:
            # 待插入节点的后继区指向原头节点
            node.next = self.__head
            # 原头节点的前驱区指向待插入节点
            self.__head.prev = node
            self.__head = node

    def append(self, value):
        node = Node(value)
        cur = self.__head
        if self.is_empty():
            self.__head = node
            return node
        while cur.next:
            cur = cur.next
        cur.next = node
        node.prev = cur
        return node

    def insert(self, pos, value):
        if pos <= 0:
            self.add(value)
        elif pos > len(self) - 1:
            self.append(value)
        else:
            # 单向链表中为了在特定位置插入，要先在链表中找到待插入位置和其前一个位置
            # 双向链表中就不需要两个游标了（当然单向链表中一个游标也是可以只找前一个位置）
            node = Node(value)
            count = 0
            cur = self.__head
            while count < pos - 1:
                count += 1
                cur = cur.next
            # 此时的游标指向pos的前一个位置
            # 这里的相互指向需尤为注意，有多种实现，需细细分析
            node.next = cur.next
            cur.next.prev = node
            node.prev = cur
            cur.next = node

    def search(self, value):
        cur = self.__head
        while cur:
            if cur.value == value:
                return True
            else:
                cur = cur.next
        return False

    def remove(self, value):
        if self.is_empty():
            return
        cur = self.__head
        while cur:
            if cur.value == value:
                if cur == self.__head:
                    self.__head = cur.next
                    # 处理链表只有一个节点的特殊情况
                    if cur.next:
                        cur.next.prev = None
                else:
                    cur.prev.next = cur.next
                    # 处理待删除节点是最后一个情况
                    if cur.next:
                        cur.next.prev = cur.prev
                return
            else:
                cur = cur.next
    
    def add_by_sort(self, act, key):
        node = Node(act)
        if self.is_empty():
            self.__head = node
            return node
        t_head = self.__head
        while t_head:
            if getattr(t_head.value, key) > getattr(node.value, key):
                if t_head.prev is not None:
                    t_head.prev.next = node
                    node.prev = t_head.prev
                    node.next = t_head
                    t_head.prev = node
                else:
                    node.next = t_head
                    t_head.prev = node
                    self.__head = node
                break

            t_head = t_head.next

        if t_head is None:
            node = self.append(act)
        return node
            