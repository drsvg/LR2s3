use dbms::lrucache::LRUCache;

fn main() {
    let mut cache = LRUCache::new();
    cache.init(2);

    cache.set("1", "2");
    cache.set("2", "3");
    cache.set("1", "5");
    cache.set("4", "5");
    cache.set("6", "7");

    println!("GET 4: {}", cache.get("4").output); // 5
    cache.set("1", "2");
    println!("GET 3: {}", cache.get("3").output); // -1

    cache.clear();
}
