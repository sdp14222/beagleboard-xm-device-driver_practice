#!/bin/sh

echo 0 > /sys/kernel/debug/tracing/tracing_on
sleep 1
echo "tracing_off"

echo function_graph > /sys/kernel/debug/tracing/current_tracer
#echo function > /sys/kernel/debug/tracing/current_tracer
sleep 1
echo "set function_graph tracer"

echo gpio_demo_isr > /sys/kernel/debug/tracing/set_ftrace_filter
echo "filter : gpio_demo_isr"
sleep 1

echo 1 > /sys/kernel/debug/tracing/tracing_on
sleep 1
echo "tracing_on"

