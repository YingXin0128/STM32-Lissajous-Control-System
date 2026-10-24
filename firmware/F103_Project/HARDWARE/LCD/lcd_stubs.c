/*
 * The vendor lcd.c demo menu calls a font-ROM self-test that is not present
 * in the SPI LCD source package.  The controller application never invokes
 * that demo menu; this no-op keeps the reusable vendor driver link-complete.
 */
void GBK_LibFont_Test(void)
{
}
