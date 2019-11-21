cmake_minimum_required(VERSION 3.2)
project(coreinit)

add_rpx(alarm/alarm_cancel.c)
add_rpx(alarm/alarm_cancel_group.c)
add_rpx(alarm/alarm_multithread.c)
add_rpx(alarm/alarm_periodic.c)
add_rpx(alarm/alarm_simple.c)
add_rpx(alarm/alarm_user_data.c)

add_rpx(coroutine/coroutine_multi.c)
add_rpx(coroutine/coroutine_single.c)

add_rpx(filesystem/filesystem_read.c)

add_rpx(memory/blockheap_simple.c)
add_rpx(memory/frameheap_multi.c)
add_rpx(memory/frameheap_simple.c)
add_rpx(memory/frameheap_unaligned.c)

add_rpx(messagequeue/messagequeue_high_priority.c)
add_rpx(messagequeue/messagequeue_peek.c)
add_rpx(messagequeue/messagequeue_peek_empty.c)
add_rpx(messagequeue/messagequeue_receive_block.c)
add_rpx(messagequeue/messagequeue_receive_empty.c)
add_rpx(messagequeue/messagequeue_send_full_block.c)
add_rpx(messagequeue/messagequeue_send_full_fail.c)
add_rpx(messagequeue/messagequeue_send_receive.c)

add_rpx(thread/thread_cancel.c)
