class Koordinat {
    private:
        double longitude;
        double latitude;
    public:
        double getLongitude() const { return longitude; }
        void setLongitude(double lon) { longitude = lon; }

        double getLatitude() const { return latitude; }
        void setLatitude(double lat) { latitude = lat; }

        Koordinat(double lon, double lat) : longitude(lon), latitude(lat) {}
};