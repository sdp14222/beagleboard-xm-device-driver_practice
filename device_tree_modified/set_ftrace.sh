#!/bin/sh

echo 0 > /sys/kernel/debug/tracing/tracing_on
sleep 1
echo "tracing_off"

# 둘 중 선택 (function_graph, function)
#echo function_graph > /sys/kernel/debug/tracing/current_tracer
#echo "set function_graph tracer"
echo function > /sys/kernel/debug/tracing/current_tracer
echo "set function tracer"
sleep 1

#echo gpio_demo_isr > /sys/kernel/debug/tracing/set_ftrace_filter
echo gpio_demo_debounce_work > /sys/kernel/debug/tracing/set_ftrace_filter
echo "filter : gpio_demo_debounce_work"
sleep 1

# func_stack_trace를 사용할 경우 function_graph가 제대로 동작하지 않는 것 같음
echo 1 > /sys/kernel/debug/tracing/options/func_stack_trace
echo "function stack trace enabled"
sleep 1


echo 1 > /sys/kernel/debug/tracing/tracing_on
sleep 1
echo "tracing_on"

