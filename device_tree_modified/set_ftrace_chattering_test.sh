#!/bin/sh

echo 0 > /sys/kernel/debug/tracing/tracing_on
sleep 1
echo "tracing_off"

echo function > /sys/kernel/debug/tracing/current_tracer
echo "set function tracer"
sleep 1

echo gpio_demo_isr > /sys/kernel/debug/tracing/set_ftrace_filter
echo "filter : gpio_demo_isr"
sleep 1

echo 1 > /sys/kernel/debug/tracing/tracing_on
sleep 1
echo "tracing_on"

