* use doubly-linked list

## Add Range to ranges
loop range in ranges:
    if new_range overlaps range:
        merge ranges
append new_range

## Merge ranges
1. update new_range with min start and max stop
2. remove old range from list
    - range->prev = range->next
    - range->next = range->prev
