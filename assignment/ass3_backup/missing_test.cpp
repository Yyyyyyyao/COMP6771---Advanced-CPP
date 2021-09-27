Case: erase edge with no edges exist
auto g = gdwg::graph<std::string, int>{"1", "2", "3"};
g.insert_edge("1", "2", 1);
g.erase_edge("3", "1", 4);


