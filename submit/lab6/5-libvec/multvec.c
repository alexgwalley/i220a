void
multvec(int a[], int b[], int z[], int n) 
{
  for (int i = 0; i < n; i++) {
    z[i] = a[i] * b[i];
  }
}
