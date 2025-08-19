module trade_type;

enum ESide { Long, Short }

struct Order
{
  string id;
  ESide side;
  double qty;
  double entry_price;
  double exit_price;
  long entry_time;
  long update_time;
}