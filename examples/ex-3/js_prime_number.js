function countPrimes(limit) {
  if (limit < 2) return 0;
  let count = 0;
  for (let i = 2; i <= limit; ++i) {
    let isPrime = true;
    for (let j = 2; j * j <= i; ++j) {
      if (i % j === 0) {
        isPrime = false;
        break;
      }
    }
    if (isPrime) ++count;
  }
  return count;
}
