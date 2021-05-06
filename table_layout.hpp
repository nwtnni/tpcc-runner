#pragma once

#include <cstdint>
#include <cstring>
#include <string_view>

template <typename T>
std::string_view to_string_view(const T& t) {
    return std::string_view(reinterpret_cast<const char*>(&t), sizeof(t));
}

struct Address {
    friend struct Warehouse;
    friend struct District;
    friend struct Customer;
    static const int MAX_STREET = 20;
    static const int MAX_CITY = 10;
    static const int STATE = 2;
    static const int ZIP = 9;
    char street_1[MAX_STREET + 1];
    char street_2[MAX_STREET + 1];
    char city[MAX_CITY + 1];
    char state[STATE + 1];
    char zip[ZIP + 1];

private:
    inline Address(){};
    inline Address& operator=(const Address& other) {
        if (this != &other) {
            strcpy(street_1, other.street_1);
            strcpy(street_2, other.street_2);
            strcpy(city, other.city);
            strcpy(state, other.state);
            strcpy(zip, other.zip);
        }
        return *this;
    }
};

struct Record {
    virtual ~Record(){};
    virtual std::string_view serialize() const = 0;
    virtual void deserialize(std::string_view value) = 0;
};

// Primary Key w_id
struct Warehouse : Record {
    static const int MAX_NAME = 10;
    uint16_t w_id;  // 2*W unique ids
    float w_tax;    // signed numeric(4, 4)
    float w_ytd;    // signed numeric(12, 2)
    char w_name[MAX_NAME + 1];
    Address w_address;
    inline std::string_view serialize() const override { return to_string_view(*this); }
    inline void deserialize(std::string_view serialized) override {
        const Warehouse* w = reinterpret_cast<const Warehouse*>(serialized.data());
        w_id = w->w_id;
        w_tax = w->w_tax;
        w_ytd = w->w_ytd;
        strcpy(w_name, w->w_name);
        w_address = w->w_address;
    }
};

// Primary Key (d_w_id, d_id)
// Foreign Key d_w_id references w_id
struct District : Record {
    static const int MAX_NAME = 10;
    uint8_t d_id;  // 20 unique ids
    uint16_t d_w_id;
    uint32_t d_next_o_id;  // 10000000 unique ids
    float d_tax;           // signed numeric(4, 4)
    float d_ytd;           // signed numeric(12, 2)
    char d_name[MAX_NAME + 1];
    Address d_address;
    inline std::string_view serialize() const override { return to_string_view(*this); }
    inline void deserialize(std::string_view serialized) override {
        const District* d = reinterpret_cast<const District*>(serialized.data());
        d_id = d->d_id;
        d_w_id = d->d_w_id;
        d_next_o_id = d->d_next_o_id;
        d_tax = d->d_tax;
        strcpy(d_name, d->d_name);
        d_address = d->d_address;
    }
};

// Primary Key (c_w_id, c_d_id, c_id)
// Foreign Key (c_w_id, c_d_id) references (d_w_id, d_id)
struct Customer : Record {
    static const int MAX_FIRST = 16;
    static const int MAX_MIDDLE = 2;
    static const int MAX_LAST = 16;
    static const int PHONE = 16;
    static const int CREDIT = 2;
    static const int MAX_DATA = 500;
    uint32_t c_id;  // 96000 unique ids
    uint8_t c_d_id;
    uint16_t c_w_id;
    uint16_t c_payment_cnt;   // numeric(4)
    uint16_t c_delivery_cnt;  // numeric(4)
    uint64_t c_since;         // date and time
    float c_credit_lim;       // signed numeric(2, 2)
    float c_discount;         // signed numeric(4, 4)
    float c_balance;          // signed numeric(12, 2)
    float c_ytd_payment;      // signed numeric(12, 2)
    char c_first[MAX_FIRST + 1];
    char c_middle[MAX_MIDDLE + 1];
    char c_last[MAX_LAST + 1];
    char c_phone[PHONE + 1];
    char c_credit[CREDIT + 1];  // "GC"=good, "BC"=bad
    char c_data[MAX_DATA + 1];  // miscellaneous information
    Address c_address;
    inline std::string_view serialize() const override { return to_string_view(*this); }
    inline void deserialize(std::string_view serialized) override {
        const Customer* c = reinterpret_cast<const Customer*>(serialized.data());
        c_id = c->c_id;
        c_d_id = c->c_d_id;
        c_w_id = c->c_w_id;
        c_payment_cnt = c->c_payment_cnt;
        c_delivery_cnt = c->c_delivery_cnt;
        c_since = c->c_since;
        c_credit_lim = c->c_credit_lim;
        c_discount = c->c_discount;
        c_balance = c->c_balance;
        c_ytd_payment = c->c_ytd_payment;
        strcpy(c_first, c->c_first);
        strcpy(c_middle, c->c_middle);
        strcpy(c_last, c->c_last);
        strcpy(c_phone, c->c_phone);
        strcpy(c_credit, c->c_credit);
        strcpy(c_data, c->c_data);
        c_address = c->c_address;
    }
};

// Primary Key None
// Foreign Key (h_c_w_id, h_c_d_id, h_c_id) references (c_w_id, c_d_id, c_id)
// Foreign Key (h_w_id, h_d_id) references (d_w_id, d_id)
struct History : Record {
    static const int MAX_DATA = 24;
    uint32_t h_c_id;
    uint8_t h_c_d_id;
    uint16_t h_c_w_id;
    uint8_t h_d_id;
    uint16_t h_w_id;
    uint64_t h_date;  // date and time
    float h_amount;   // signed numeric(6, 2)
    char h_data[MAX_DATA + 1];
    inline std::string_view serialize() const override { return to_string_view(*this); }
    inline void deserialize(std::string_view serialized) override {
        const History* h = reinterpret_cast<const History*>(serialized.data());
        h_c_id = h->h_c_id;
        h_c_d_id = h->h_c_d_id;
        h_c_w_id = h->h_c_w_id;
        h_d_id = h->h_d_id;
        h_w_id = h->h_w_id;
        h_date = h->h_date;
        h_amount = h->h_amount;
        strcpy(h_data, h->h_data);
    }
};

// Primary Key (o_w_id, o_d_id, o_id)
// Foreign Key (o_w_id, o_d_id, o_c_id) references (c_w_id, c_d_id, c_id)
struct Order : Record {
    uint32_t o_id;  // 10000000 unique ids
    uint8_t o_d_id;
    uint16_t o_w_id;
    uint32_t o_c_id;
    uint8_t o_carrier_id;  // 10 unique ids or null
    uint8_t o_ol_cnt;      // numeric(2)
    uint8_t o_all_local;   // numeric(1)
    uint64_t o_entry_d;    // date and time
    inline std::string_view serialize() const override { return to_string_view(*this); }
    inline void deserialize(std::string_view seralized) override {
        const Order* o = reinterpret_cast<const Order*>(seralized.data());
        o_id = o->o_id;
        o_d_id = o->o_d_id;
        o_w_id = o->o_w_id;
        o_c_id = o->o_c_id;
        o_carrier_id = o->o_carrier_id;
        o_ol_cnt = o->o_ol_cnt;
        o_all_local = o->o_all_local;
        o_entry_d = o->o_entry_d;
    }
};

// Primary Key (no_w_id, no_d_id, no_o_id)
// Foreign Key (no_w_id, no_d_id, no_o_id) references (o_w_id, o_d_id, o_id)
struct NewOrder : Record {
    uint32_t no_o_id;
    uint8_t no_d_id;
    uint16_t no_w_id;
    inline std::string_view serialize() const override { return to_string_view(*this); }
    inline void deserialize(std::string_view serialized) override {
        const NewOrder* n = reinterpret_cast<const NewOrder*>(serialized.data());
        no_o_id = n->no_o_id;
        no_d_id = n->no_d_id;
        no_w_id = n->no_w_id;
    }
};

// Primary Key (ol_w_id, ol_d_id, ol_o_id, ol_number)
// Foregin Key (ol_w_id, ol_d_id, ol_o_id) references (o_w_id, o_d_id, o_id)
// Foreign Key (ol_supply_w_id, ol_i_id) references (s_w_id, s_i_id)
struct OrderLine : Record {
    static const int DIST_INFO = 24;
    uint32_t ol_o_id;
    uint8_t ol_d_id;
    uint16_t ol_w_id;
    uint8_t ol_number;  // 15 unique ids
    uint32_t ol_i_id;   // 200000 unique ids
    uint16_t ol_supply_w_id;
    uint8_t ol_quantity;  // numeric(2)
    float ol_amount;      // signed numeric(6, 2)
    char ol_dist_info[DIST_INFO + 1];
    inline std::string_view serialize() const override { return to_string_view(*this); }
    inline void deserialize(std::string_view serialized) override {
        const OrderLine* o = reinterpret_cast<const OrderLine*>(serialized.data());
        ol_o_id = o->ol_o_id;
        ol_d_id = o->ol_d_id;
        ol_w_id = o->ol_w_id;
        ol_number = o->ol_number;
        ol_i_id = o->ol_i_id;
        ol_supply_w_id = o->ol_supply_w_id;
        ol_quantity = o->ol_quantity;
        ol_amount = o->ol_amount;
        strcpy(ol_dist_info, o->ol_dist_info);
    }
};

// Primary Key i_id
struct Item : Record {
    static const int MAX_NAME = 24;
    static const int MAX_DATA = 50;
    uint32_t i_id;     // 200000 unique ids
    uint32_t i_im_id;  // 200000 unique ids
    float i_price;     // numeric(5, 2)
    char i_name[MAX_NAME + 1];
    char i_data[MAX_DATA + 1];
    inline std::string_view serialize() const override { return to_string_view(*this); }
    inline void deserialize(std::string_view serialized) override {
        const Item* i = reinterpret_cast<const Item*>(serialized.data());
        i_id = i->i_id;
        i_im_id = i->i_im_id;
        i_price = i->i_price;
        strcpy(i_name, i->i_name);
        strcpy(i_data, i->i_data);
    }
};

// Primary Key (s_w_id, s_i_id)
// Foreign Key s_w_id references w_id
struct Stock : Record {
    static const int DIST = 24;
    static const int MAX_DATA = 50;
    uint32_t s_i_id;  // 200000 unique ids
    uint16_t s_w_id;
    int16_t s_quantity;     // signed numeric(4)
    uint32_t s_ytd;         // numeric(8)
    uint16_t s_order_cnt;   // numeric(4)
    uint16_t s_remote_cnt;  // numeric(4)
    char s_dist_01[DIST + 1];
    char s_dist_02[DIST + 1];
    char s_dist_03[DIST + 1];
    char s_dist_04[DIST + 1];
    char s_dist_05[DIST + 1];
    char s_dist_06[DIST + 1];
    char s_dist_07[DIST + 1];
    char s_dist_08[DIST + 1];
    char s_dist_09[DIST + 1];
    char s_dist_10[DIST + 1];
    char s_data[MAX_DATA + 1];
    inline std::string_view serialize() const override { return to_string_view(*this); }
    inline void deserialize(std::string_view serialized) override {
        const Stock* s = reinterpret_cast<const Stock*>(serialized.data());
        s_i_id = s->s_i_id;
        s_w_id = s->s_w_id;
        s_quantity = s->s_quantity;
        s_ytd = s->s_ytd;
        s_order_cnt = s->s_order_cnt;
        s_remote_cnt = s->s_remote_cnt;
        strcpy(s_dist_01, s->s_dist_01);
        strcpy(s_dist_02, s->s_dist_02);
        strcpy(s_dist_03, s->s_dist_03);
        strcpy(s_dist_04, s->s_dist_04);
        strcpy(s_dist_05, s->s_dist_05);
        strcpy(s_dist_06, s->s_dist_06);
        strcpy(s_dist_07, s->s_dist_07);
        strcpy(s_dist_08, s->s_dist_08);
        strcpy(s_dist_09, s->s_dist_09);
        strcpy(s_dist_10, s->s_dist_10);
        strcpy(s_data, s->s_data);
    }
};
