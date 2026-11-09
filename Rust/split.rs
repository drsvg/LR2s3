use crate::common::*;
use crate::set::Set;

fn backtrack(nums: &[i32], idx: usize, sums: &mut Vec<i32>, bins: &mut Vec<Vec<i32>>, target: i32) -> bool {
    if idx == nums.len() {
        return sums.iter().all(|&s| s == target);
    }
    let v = nums[idx];
    for i in 0..sums.len() {
        if sums[i] + v > target { continue; }
        sums[i] += v;
        bins[i].push(v);
        if backtrack(nums, idx + 1, sums, bins, target) { return true; }
        sums[i] -= v;
        bins[i].pop();
        if sums[i] == 0 { break; }
        if sums[i] + v == target { break; }
    }
    false
}

pub fn split_set(s: &Set, target: i32) -> CommandResult {
    if target <= 0 { return make_error("ERROR: target must be positive"); }

    let mut nums: Vec<i32> = Vec::new();
    let mut total = 0;
    for v in &s.items {
        match v.parse::<i32>() {
            Ok(n) => {
                if n <= 0 { return make_error("ERROR: set must contain positive naturals"); }
                nums.push(n); total += n;
            }
            Err(_) => return make_error("ERROR: non-integer value in set"),
        }
    }
    if nums.is_empty() { return make_error("ERROR: set is empty"); }
    if total % target != 0 { return make_error("ERROR: sum is not divisible by target"); }

    let k = (total / target) as usize;
    if k > nums.len() { return make_error("ERROR: too many subsets for this set size"); }

    nums.sort_by(|a, b| b.cmp(a));
    if nums[0] > target { return make_error("ERROR: element exceeds target sum"); }

    let mut sums = vec![0; k];
    let mut bins: Vec<Vec<i32>> = vec![Vec::new(); k];
    if !backtrack(&nums, 0, &mut sums, &mut bins, target) {
        return make_error("ERROR: impossible to split");
    }

    let parts: Vec<String> = bins.iter().map(|b| {
        format!("{{{}}}", b.iter().map(|v| v.to_string()).collect::<Vec<_>>().join(","))
    }).collect();
    make_ok(parts.join(" "))
}