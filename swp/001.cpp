void LineTo(int x, int y, int x1, int y1)
{
	int d, dx, dy; double kx, ky;
	COLORREF color = getcolor();
	moveto(x, y); 

	dx =(int)(x - x1)*(x-x1);
	dy =(int)(y - y1)*(y-y1);
	d =(int)sqrt(dx+dy); //计算两点的距离

	kx = (x1 - x)/d;
	ky = (y1 - y)/d;

	for (int i = 0; i <= d; i++)
	{
		dx = x + kx * i;
		dy = y + ky * i;
		putpixel(dx, dy, color);
	}
}
