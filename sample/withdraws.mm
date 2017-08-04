#
# Description:
#   Took this example straight out of the-wizard-book
#   (the chapter on local state and modularity)
#

make_account = balance -> (() -> do
  balance = balance;
  withdraw = amount -> {
    @"Cannot withdraw negative value"   if amount < 0,
    balance <- balance - amount         if balance >= amount,
    @"Insufficient funds"               else
  };
  deposit = amount -> {
    @"Cannot deposit negative value"   if amount < 0,
    balance <- balance + amount        else
  };

  m -> {
    withdraw                   if m == @withdraw or m == &withdraw,
    deposit                    if m == @deposit or m == &deposit,
    balance                    if m == @balance or m == &balance,
    error(@"ILLEGAL ACTION")   else
  }
end)();

account1 = make_account(100);
account2 = make_account(100);

print(@"Withdraw 60 bucks from account 1: ");
print(account1(&withdraw, 60)); newline();

print(@"Withdraw 60 bucks from account 1: ");
print(account1(&withdraw, 60)); newline();

print(@"Deposit 5 bucks from account 1: ");
print(account1(&deposit, 5)); newline();

print(@"Withdraw 70 bucks from account 2: ");
print(account2(@withdraw, 70)); newline();

@DONE
