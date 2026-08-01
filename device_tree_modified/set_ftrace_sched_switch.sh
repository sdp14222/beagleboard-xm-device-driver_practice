#!/bin/sh

echo 0 > /sys/kernel/debug/tracing/tracing_on
sleep 1
echo "tracing_off"

echo function > /sys/kernel/debug/tracing/current_tracer
echo "current tracer : function"
sleep 1

echo gpio_demo_hard_isr > /sys/kernel/debug/tracing/set_ftrace_filter
echo "set_trace_filter : gpio_demo_hard_isr"
echo gpio_demo_debounce_work >> /sys/kernel/debug/tracing/set_ftrace_filter
echo "set_trace_filter : gpio_demo_debounce_work"

echo 1 > /sys/kernel/debug/tracing/events/sched/sched_switch/enable
echo "sched_switch enabled"
sleep 1

echo > /sys/kernel/debug/tracing/trace
sleep 1

echo 1 > /sys/kernel/debug/tracing/tracing_on
sleep 1
echo "tracing_on"

