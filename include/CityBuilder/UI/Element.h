
struct Element {
  virtual void setPadding(Real4 padding) = 0;
  virtual Real4 getPadding() = 0;

  virtual void setBorderWidth(Real width) = 0;
  virtual Real getBorderWidth() = 0;

  virtual void setBorderRadius(Real radius) = 0;
  virtual Real getBorderRadius() = 0;

  virtual void setBorderColor(Color4 color) = 0;
  virtual Color4 getBorderColor() = 0;

  virtual void setMargin(Real4 margin) = 0;
  virtual Real4 getMargin() = 0;
}
