#include <cstddef>
#include <cstdlib>
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <set>
#include <string>
#include <fstream>
#include "turing_machine.h"
using namespace std;

vector<string> blank_vector = {BLANK};
int counter = 0;

int get_next_number()
{
    counter++;
    return counter;
}

string remove_char_from_string(string str, char a)
{
    string res = "";
    for (char ch : str)
    {
        if (ch != a)
        {
            res += ch;
        }
    }
    return res;
}

string remove_brackets_from_string(string str)
{
    return remove_char_from_string(remove_char_from_string(str, '('), ')');
}

string find_the_longest(vector<string> input_alphabet)
{
    unsigned int max_size = 0, index = -1;
    for (unsigned int i = 0; i < input_alphabet.size(); i++)
    {
        auto single = input_alphabet[i];
        if (max_size < single.size())
        {
            index = i;
            max_size = single.size();
        }
    }
    return remove_brackets_from_string(input_alphabet[index]);
}

string create_splitter(vector<string> input_alphabet)
{
    string longest = find_the_longest(input_alphabet);
    return "(" + longest + longest + ")";
}

string create_head(vector<string> input_alphabet)
{
    string longest = find_the_longest(input_alphabet);
    return "(" + longest + longest + longest + ")";
}

TuringMachine create_empty_from_input(TuringMachine input_tm)
{
    vector<string> input_alphabet = input_tm.input_alphabet;
    transitions_t transitions;

    return TuringMachine(1, input_alphabet, transitions);
}

TuringMachine prepare_single_tape_machine(TuringMachine tm, string splitter, string head, vector<string> transition_alphabet)
{
    for (string single_letter : transition_alphabet)
    {
        vector<string> single_letter_vector = {single_letter};
        pair<string, vector<string>> input_transition = make_pair(INITIAL_STATE, single_letter_vector);
        vector<string> head_vector = {head};
        tuple<string, vector<string>, string> output_transition = make_tuple("(" + remove_brackets_from_string(single_letter) + "start1" + remove_brackets_from_string(single_letter) + ")", head_vector, ">");
        tm.transitions[input_transition] = output_transition;
    }
    for (string global_letter : transition_alphabet)
    {
        for (string single_letter : transition_alphabet)
        {
            for (string secong_single_letter : transition_alphabet)
            {
                vector<string> second_single_letter_vector = {secong_single_letter};
                pair<string, vector<string>> input_transition = make_pair("(" + remove_brackets_from_string(global_letter) + "start1" + remove_brackets_from_string(single_letter) + ")", second_single_letter_vector);
                vector<string> single_letter_vector = {single_letter};
                tuple<string, vector<string>, string> output_transition = make_tuple("(" + remove_brackets_from_string(global_letter) + "start1" + remove_brackets_from_string(secong_single_letter) + ")", single_letter_vector, ">");
                tm.transitions[input_transition] = output_transition;
            }

            pair<string, vector<string>> input_transition = make_pair("(" + remove_brackets_from_string(global_letter) + "start1" + remove_brackets_from_string(single_letter) + ")", blank_vector);
            vector<string> single_letter_vector = {single_letter};
            tuple<string, vector<string>, string> output_transition = make_tuple("(" + remove_brackets_from_string(global_letter) + "start2)", single_letter_vector, ">");
            tm.transitions[input_transition] = output_transition;

            input_transition = make_pair("(" + remove_brackets_from_string(global_letter) + "start7)", single_letter_vector);
            output_transition = make_tuple("(" + remove_brackets_from_string(global_letter) + "start7)", single_letter_vector, "<");
            tm.transitions[input_transition] = output_transition;
        }

        pair<string, vector<string>> input_transition = make_pair("(" + remove_brackets_from_string(global_letter) + "start2)", blank_vector);
        vector<string> splitter_vector = {splitter};
        tuple<string, vector<string>, string> output_transition = make_tuple("(" + remove_brackets_from_string(global_letter) + "start3)", splitter_vector, ">");
        tm.transitions[input_transition] = output_transition;

        input_transition = make_pair("(" + remove_brackets_from_string(global_letter) + "start3)", blank_vector);
        vector<string> head_vector = {head};
        output_transition = make_tuple("(" + remove_brackets_from_string(global_letter) + "start3_5)", head_vector, ">");
        tm.transitions[input_transition] = output_transition;

        input_transition = make_pair("(" + remove_brackets_from_string(global_letter) + "start3_5)", blank_vector);
        output_transition = make_tuple("(" + remove_brackets_from_string(global_letter) + "start4)", blank_vector, ">");
        tm.transitions[input_transition] = output_transition;

        input_transition = make_pair("(" + remove_brackets_from_string(global_letter) + "start4)", blank_vector);
        output_transition = make_tuple("(" + remove_brackets_from_string(global_letter) + "start4_5)", splitter_vector, "<");
        tm.transitions[input_transition] = output_transition;

        input_transition = make_pair("(" + remove_brackets_from_string(global_letter) + "start4_5)", blank_vector);
        output_transition = make_tuple("(" + remove_brackets_from_string(global_letter) + "start5)", blank_vector, "<");
        tm.transitions[input_transition] = output_transition;

        input_transition = make_pair("(" + remove_brackets_from_string(global_letter) + "start5)", head_vector);
        output_transition = make_tuple("(" + remove_brackets_from_string(global_letter) + "start6)", head_vector, "<");
        tm.transitions[input_transition] = output_transition;

        input_transition = make_pair("(" + remove_brackets_from_string(global_letter) + "start6)", splitter_vector);
        output_transition = make_tuple("(" + remove_brackets_from_string(global_letter) + "start7)", splitter_vector, "<");
        tm.transitions[input_transition] = output_transition;

        string temp_splitter = remove_brackets_from_string(splitter);

        input_transition = make_pair("(" + remove_brackets_from_string(global_letter) + "start7)", head_vector);
        output_transition = make_tuple("(" + remove_brackets_from_string(global_letter) + temp_splitter + "_" + temp_splitter + "START)", head_vector, "-");
        tm.transitions[input_transition] = output_transition;
    }

    return tm;
}

TuringMachine add_all_alphabet_and_splitter_states(TuringMachine tm, string splitter, string state1, string state2, string direction, vector<string> transition_alphabet)
{
    pair<string, vector<string>> input_transition = make_pair(state1, blank_vector);
    tuple<string, vector<string>, string> output_transition = make_tuple(state2, blank_vector, direction);
    tm.transitions[input_transition] = output_transition;

    vector<string> splitter_vector = {splitter};
    input_transition = make_pair(state1, splitter_vector);
    output_transition = make_tuple(state2, splitter_vector, direction);
    tm.transitions[input_transition] = output_transition;

    for (string single_letter : transition_alphabet)
    {
        vector<string> single_letter_vector = {single_letter};
        input_transition = make_pair(state1, single_letter_vector);
        output_transition = make_tuple(state2, single_letter_vector, direction);
        tm.transitions[input_transition] = output_transition;
    }
    return tm;
}

TuringMachine add_state_to_tm(TuringMachine tm, string a, string b, string state1, string state2, string direction)
{
    vector<string> a_vector = {a};
    vector<string> b_vector = {b};

    pair<string, vector<string>> input_transition = make_pair(state1, a_vector);
    tuple<string, vector<string>, string> output_transition = make_tuple(state2, b_vector, direction);
    tm.transitions[input_transition] = output_transition;

    return tm;
}

TuringMachine move_tape_by_one(TuringMachine tm, string splitter, string head, string start_state, string state2, vector<string> temp_alphabet, int this_state_number)
{
    string state_p = "(" + start_state + to_string(this_state_number) + "P" + ")";
    tm = add_state_to_tm(tm, splitter, BLANK, state2, state_p, ">");
    string state_p_hash = "(" + start_state + to_string(this_state_number) + "P" + "hash" + ")";
    temp_alphabet.push_back(head);
    for (string single_letter : temp_alphabet)
    {
        string state_p2_letter = "(" + start_state + to_string(this_state_number) + "P2" + remove_brackets_from_string(single_letter) + ")";
        tm = add_state_to_tm(tm, single_letter, splitter, state_p, state_p2_letter, ">");
        for (string second_single_letter : temp_alphabet)
        {
            string state_p2_second_letter = "(" + start_state + to_string(this_state_number) + "P2" + remove_brackets_from_string(second_single_letter) + ")";
            tm = add_state_to_tm(tm, second_single_letter, single_letter, state_p2_letter, state_p2_second_letter, ">");
        }
        tm = add_state_to_tm(tm, splitter, single_letter, state_p2_letter, state_p_hash, ">");
    }
    string state_r = "(" + start_state + to_string(this_state_number) + "R" + ")";
    string state_r2 = "(" + start_state + to_string(this_state_number) + "R2" + ")";

    tm = add_state_to_tm(tm, BLANK, splitter, state_p_hash, state_r, "<");

    tm = add_all_alphabet_and_splitter_states(tm, splitter, state_r, state_r, "<", temp_alphabet);
    tm = add_state_to_tm(tm, BLANK, BLANK, state_r, state_r, "<");

    tm = add_state_to_tm(tm, head, head, state_r, state_r2, "<");
    tm = add_all_alphabet_and_splitter_states(tm, splitter, state_r2, state_r2, "<", temp_alphabet);
    tm = add_state_to_tm(tm, BLANK, BLANK, state_r2, state_r2, "<");

    tm = add_state_to_tm(tm, head, head, state_r2, state2, ">");

    return tm;
}

TuringMachine add_right_left_states(TuringMachine tm, string splitter, string head, pair<string, vector<string>> input_state, tuple<string, vector<string>, string> output_state, vector<string> transition_alphabet)
{
    string a = input_state.second[0];
    string b = input_state.second[1];
    string c = get<1>(output_state)[0];
    string d = get<1>(output_state)[1];
    string start_state = input_state.first;
    string end_state = get<0>(output_state);

    if (start_state == INITIAL_STATE)
    {
        start_state = "(START)";
    }

    if (end_state == INITIAL_STATE)
    {
        end_state = "(START)";
    }

    int this_state_number = get_next_number();

    string temp_splitter = remove_brackets_from_string(splitter);
    string state_prefix = remove_brackets_from_string(a) + temp_splitter + remove_brackets_from_string(b) + temp_splitter;

    string temp_start_state = "(" + state_prefix + remove_brackets_from_string(start_state) + ")";

    string state1 = "(" + remove_brackets_from_string(start_state) + to_string(this_state_number) + to_string(1) + ")";
    tm = add_state_to_tm(tm, head, c, temp_start_state, state1, ">");

    start_state = remove_brackets_from_string(start_state);

    string state2 = "(" + start_state + to_string(this_state_number) + to_string(2) + ")";
    tm = add_state_to_tm(tm, a, head, state1, state2, ">");

    for (string single_letter : transition_alphabet)
    {
        string state3 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(3) + ")";
        tm = add_state_to_tm(tm, single_letter, single_letter, state2, state3, ">");
        tm = add_all_alphabet_and_splitter_states(tm, splitter, state3, state3, ">", transition_alphabet);

        string state4 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(4) + ")";
        tm = add_state_to_tm(tm, head, head, state3, state4, ">");

        string state5 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(5) + ")";
        tm = add_state_to_tm(tm, b, d, state4, state5, "<");

        string state6 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(6) + ")";
        tm = add_state_to_tm(tm, head, head, state5, state6, "<");

        for (string second_single_letter : transition_alphabet)
        {
            string state7_letter = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(second_single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(7) + ")";
            tm = add_state_to_tm(tm, second_single_letter, head, state6, state7_letter, ">");

            string state8 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(second_single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(8) + ")";
            tm = add_state_to_tm(tm, head, second_single_letter, state7_letter, state8, "<");

            string state9 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(second_single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(9) + ")";
            tm = add_state_to_tm(tm, head, head, state8, state9, "<");

            tm = add_all_alphabet_and_splitter_states(tm, splitter, state9, state9, "<", transition_alphabet);

            string temp_end_state = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(second_single_letter) + temp_splitter + remove_brackets_from_string(end_state) + ")";
            if (end_state == "(accept)" || end_state == "(reject)")
            {
                temp_end_state = end_state;
            }
            tm = add_state_to_tm(tm, head, head, state9, temp_end_state, "-");
        }
    }
    tm = move_tape_by_one(tm, splitter, head, start_state, state2, transition_alphabet, this_state_number);

    return tm;
}

TuringMachine add_right_no_states(TuringMachine tm, string splitter, string head, pair<string, vector<string>> input_state, tuple<string, vector<string>, string> output_state, vector<string> transition_alphabet)
{
    string a = input_state.second[0];
    string b = input_state.second[1];
    string c = get<1>(output_state)[0];
    string d = get<1>(output_state)[1];
    string start_state = input_state.first;
    string end_state = get<0>(output_state);

    if (start_state == INITIAL_STATE)
    {
        start_state = "(START)";
    }

    if (end_state == INITIAL_STATE)
    {
        end_state = "(START)";
    }

    int this_state_number = get_next_number();

    string temp_splitter = remove_brackets_from_string(splitter);
    string state_prefix = remove_brackets_from_string(a) + temp_splitter + remove_brackets_from_string(b) + temp_splitter;

    string temp_start_state = "(" + state_prefix + remove_brackets_from_string(start_state) + ")";

    string state1 = "(" + remove_brackets_from_string(start_state) + to_string(this_state_number) + to_string(1) + ")";
    tm = add_state_to_tm(tm, head, c, temp_start_state, state1, ">");

    start_state = remove_brackets_from_string(start_state);

    string state2 = "(" + start_state + to_string(this_state_number) + to_string(2) + ")";
    tm = add_state_to_tm(tm, a, head, state1, state2, ">");

    for (string single_letter : transition_alphabet)
    {
        string state3 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(d) + temp_splitter + start_state + to_string(this_state_number) + to_string(3) + ")";
        tm = add_state_to_tm(tm, single_letter, single_letter, state2, state3, ">");

        tm = add_all_alphabet_and_splitter_states(tm, splitter, state3, state3, ">", transition_alphabet);

        string state4 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(d) + temp_splitter + start_state + to_string(this_state_number) + to_string(4) + ")";
        tm = add_state_to_tm(tm, head, head, state3, state4, ">");

        string state5 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(d) + temp_splitter + start_state + to_string(this_state_number) + to_string(5) + ")";
        tm = add_state_to_tm(tm, b, d, state4, state5, "<");

        string state6 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(d) + temp_splitter + start_state + to_string(this_state_number) + to_string(6) + ")";
        tm = add_state_to_tm(tm, head, head, state5, state6, "<");

        tm = add_all_alphabet_and_splitter_states(tm, splitter, state6, state6, "<", transition_alphabet);

        string temp_end_state = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(d) + temp_splitter + remove_brackets_from_string(end_state) + ")";
        if (end_state == "(accept)" || end_state == "(reject)")
        {
            temp_end_state = end_state;
        }
        tm = add_state_to_tm(tm, head, head, state6, temp_end_state, "-");
    }
    tm = move_tape_by_one(tm, splitter, head, start_state, state2, transition_alphabet, this_state_number);

    return tm;
}

TuringMachine add_right_right_states(TuringMachine tm, string splitter, string head, pair<string, vector<string>> input_state, tuple<string, vector<string>, string> output_state, vector<string> transition_alphabet)
{
    string a = input_state.second[0];
    string b = input_state.second[1];
    string c = get<1>(output_state)[0];
    string d = get<1>(output_state)[1];
    string start_state = input_state.first;
    string end_state = get<0>(output_state);

    if (start_state == INITIAL_STATE)
    {
        start_state = "(START)";
    }

    if (end_state == INITIAL_STATE)
    {
        end_state = "(START)";
    }

    int this_state_number = get_next_number();

    string temp_splitter = remove_brackets_from_string(splitter);
    string state_prefix = remove_brackets_from_string(a) + temp_splitter + remove_brackets_from_string(b) + temp_splitter;

    string temp_start_state = "(" + state_prefix + remove_brackets_from_string(start_state) + ")";

    string state1 = "(" + remove_brackets_from_string(start_state) + to_string(this_state_number) + to_string(1) + ")";
    tm = add_state_to_tm(tm, head, c, temp_start_state, state1, ">");

    start_state = remove_brackets_from_string(start_state);

    string state2 = "(" + start_state + to_string(this_state_number) + to_string(2) + ")";
    tm = add_state_to_tm(tm, a, head, state1, state2, ">");

    for (string single_letter : transition_alphabet)
    {
        string state3 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(3) + ")";
        tm = add_state_to_tm(tm, single_letter, single_letter, state2, state3, ">");

        tm = add_all_alphabet_and_splitter_states(tm, splitter, state3, state3, ">", transition_alphabet);

        string state4 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(4) + ")";
        tm = add_state_to_tm(tm, head, d, state3, state4, ">");

        string state5 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(5) + ")";
        tm = add_state_to_tm(tm, b, head, state4, state5, ">");

        for (string second_single_letter : transition_alphabet)
        {
            string state6 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(second_single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(6) + ")";
            tm = add_state_to_tm(tm, second_single_letter, second_single_letter, state5, state6, "<");

            string state7 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(second_single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(7) + ")";
            tm = add_state_to_tm(tm, head, head, state6, state7, "<");

            tm = add_all_alphabet_and_splitter_states(tm, splitter, state7, state7, "<", transition_alphabet);

            string temp_end_state = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(second_single_letter) + temp_splitter + remove_brackets_from_string(end_state) + ")";
            if (end_state == "(accept)" || end_state == "(reject)")
            {
                temp_end_state = end_state;
            }
            tm = add_state_to_tm(tm, head, head, state7, temp_end_state, "-");
        }
        string state5_5 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + "_" + temp_splitter + start_state + to_string(this_state_number) + to_string(55) + ")";
        tm = add_state_to_tm(tm, splitter, BLANK, state5, state5_5, ">");
        string state5_75 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + "_" + temp_splitter + start_state + to_string(this_state_number) + to_string(575) + ")";
        tm = add_state_to_tm(tm, BLANK, splitter, state5_5, state5_75, "<");
        tm = add_state_to_tm(tm, BLANK, BLANK, state5_75, "(" + remove_brackets_from_string(single_letter) + temp_splitter + "_" + temp_splitter + start_state + to_string(this_state_number) + to_string(6) + ")", "<");
    }
    tm = move_tape_by_one(tm, splitter, head, start_state, state2, transition_alphabet, this_state_number);

    return tm;
}

TuringMachine add_left_right_states(TuringMachine tm, string splitter, string head, pair<string, vector<string>> input_state, tuple<string, vector<string>, string> output_state, vector<string> transition_alphabet)
{
    string a = input_state.second[0];
    string b = input_state.second[1];
    string c = get<1>(output_state)[0];
    string d = get<1>(output_state)[1];
    string start_state = input_state.first;
    string end_state = get<0>(output_state);

    if (start_state == INITIAL_STATE)
    {
        start_state = "(START)";
    }

    if (end_state == INITIAL_STATE)
    {
        end_state = "(START)";
    }

    int this_state_number = get_next_number();

    string temp_splitter = remove_brackets_from_string(splitter);
    string state_prefix = remove_brackets_from_string(a) + temp_splitter + remove_brackets_from_string(b) + temp_splitter;

    string temp_start_state = "(" + state_prefix + remove_brackets_from_string(start_state) + ")";

    string state1 = "(" + remove_brackets_from_string(start_state) + to_string(this_state_number) + to_string(1) + ")";
    tm = add_state_to_tm(tm, head, head, temp_start_state, state1, ">");

    start_state = remove_brackets_from_string(start_state);

    string state2 = "(" + start_state + to_string(this_state_number) + to_string(2) + ")";
    tm = add_state_to_tm(tm, a, c, state1, state2, "<");

    string state3 = "(" + start_state + to_string(this_state_number) + to_string(3) + ")";
    tm = add_state_to_tm(tm, head, head, state2, state3, "<");

    for (string single_letter : transition_alphabet)
    {
        string state4_letter = "(" + remove_brackets_from_string(start_state) + to_string(this_state_number) + to_string(4) + remove_brackets_from_string(single_letter) + ")";
        string state5 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(5) + ")";

        tm = add_state_to_tm(tm, single_letter, head, state3, state4_letter, ">");

        tm = add_state_to_tm(tm, head, single_letter, state4_letter, state5, ">");

        tm = add_all_alphabet_and_splitter_states(tm, splitter, state5, state5, ">", transition_alphabet);

        string state6 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(6) + ")";
        tm = add_state_to_tm(tm, head, d, state5, state6, ">");

        string state7 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(7) + ")";
        tm = add_state_to_tm(tm, b, head, state6, state7, ">");

        for (string second_single_letter : transition_alphabet)
        {
            string state8 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(second_single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(8) + ")";
            string state9 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(second_single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(9) + ")";

            tm = add_state_to_tm(tm, second_single_letter, second_single_letter, state7, state8, "<");

            tm = add_all_alphabet_and_splitter_states(tm, splitter, state9, state9, "<", transition_alphabet);

            tm = add_state_to_tm(tm, head, head, state8, state9, "<");

            string temp_end_state = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(second_single_letter) + temp_splitter + remove_brackets_from_string(end_state) + ")";
            if (end_state == "(accept)" || end_state == "(reject)")
            {
                temp_end_state = end_state;
            }
            tm = add_state_to_tm(tm, head, head, state9, temp_end_state, "-");
        }
        string state7_5 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + "_" + temp_splitter + start_state + to_string(this_state_number) + to_string(75) + ")";
        tm = add_state_to_tm(tm, splitter, BLANK, state7, state7_5, ">");
        string state7_75 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + "_" + temp_splitter + start_state + to_string(this_state_number) + to_string(775) + ")";
        tm = add_state_to_tm(tm, BLANK, splitter, state7_5, state7_75, "<");
        tm = add_state_to_tm(tm, BLANK, BLANK, state7_75, "(" + remove_brackets_from_string(single_letter) + temp_splitter + "_" + temp_splitter + start_state + to_string(this_state_number) + to_string(8) + ")", "<");
    }

    return tm;
}

TuringMachine add_left_left_states(TuringMachine tm, string splitter, string head, pair<string, vector<string>> input_state, tuple<string, vector<string>, string> output_state, vector<string> transition_alphabet)
{
    string a = input_state.second[0];
    string b = input_state.second[1];
    string c = get<1>(output_state)[0];
    string d = get<1>(output_state)[1];
    string start_state = input_state.first;
    string end_state = get<0>(output_state);

    if (start_state == INITIAL_STATE)
    {
        start_state = "(START)";
    }

    if (end_state == INITIAL_STATE)
    {
        end_state = "(START)";
    }

    int this_state_number = get_next_number();

    string temp_splitter = remove_brackets_from_string(splitter);
    string state_prefix = remove_brackets_from_string(a) + temp_splitter + remove_brackets_from_string(b) + temp_splitter;

    string temp_start_state = "(" + state_prefix + remove_brackets_from_string(start_state) + ")";

    string state1 = "(" + remove_brackets_from_string(start_state) + to_string(this_state_number) + to_string(1) + ")";
    tm = add_state_to_tm(tm, head, head, temp_start_state, state1, ">");

    start_state = remove_brackets_from_string(start_state);

    string state2 = "(" + start_state + to_string(this_state_number) + to_string(2) + ")";
    tm = add_state_to_tm(tm, a, c, state1, state2, "<");

    string state3 = "(" + start_state + to_string(this_state_number) + to_string(3) + ")";
    tm = add_state_to_tm(tm, head, head, state2, state3, "<");

    for (string single_letter : transition_alphabet)
    {
        string state4_letter = "(" + start_state + to_string(this_state_number) + to_string(4) + remove_brackets_from_string(single_letter) + ")";
        string state5 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(5) + ")";

        tm = add_state_to_tm(tm, single_letter, head, state3, state4_letter, ">");

        tm = add_state_to_tm(tm, head, single_letter, state4_letter, state5, ">");

        string state4_blank = "(" + remove_brackets_from_string(single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(4) + BLANK + ")";

        tm = add_all_alphabet_and_splitter_states(tm, splitter, state5, state5, ">", transition_alphabet);

        string state6 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(6) + ")";
        tm = add_state_to_tm(tm, head, head, state5, state6, ">");

        string state7 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(7) + ")";
        tm = add_state_to_tm(tm, b, d, state6, state7, "<");

        string state8 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(8) + ")";
        tm = add_state_to_tm(tm, head, head, state7, state8, "<");

        for (string second_single_letter : transition_alphabet)
        {
            string state9_letter = "(" + remove_brackets_from_string(single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(9) + remove_brackets_from_string(second_single_letter) + ")";
            string state10 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(second_single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(10) + ")";
            string state11 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(second_single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(11) + ")";

            tm = add_state_to_tm(tm, second_single_letter, head, state8, state9_letter, ">");

            tm = add_state_to_tm(tm, head, second_single_letter, state9_letter, state10, "<");

            tm = add_state_to_tm(tm, head, head, state10, state11, "<");

            tm = add_all_alphabet_and_splitter_states(tm, splitter, state11, state11, "<", transition_alphabet);

            string temp_end_state = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(second_single_letter) + temp_splitter + remove_brackets_from_string(end_state) + ")";
            if (end_state == "(accept)" || end_state == "(reject)")
            {
                temp_end_state = end_state;
            }
            tm = add_state_to_tm(tm, head, head, state11, temp_end_state, "-");
        }
    }

    return tm;
}

TuringMachine add_left_no_states(TuringMachine tm, string splitter, string head, pair<string, vector<string>> input_state, tuple<string, vector<string>, string> output_state, vector<string> transition_alphabet)
{
    string a = input_state.second[0];
    string b = input_state.second[1];
    string c = get<1>(output_state)[0];
    string d = get<1>(output_state)[1];
    string start_state = input_state.first;
    string end_state = get<0>(output_state);

    if (start_state == INITIAL_STATE)
    {
        start_state = "(START)";
    }

    if (end_state == INITIAL_STATE)
    {
        end_state = "(START)";
    }

    int this_state_number = get_next_number();

    string temp_splitter = remove_brackets_from_string(splitter);
    string state_prefix = remove_brackets_from_string(a) + temp_splitter + remove_brackets_from_string(b) + temp_splitter;

    string temp_start_state = "(" + state_prefix + remove_brackets_from_string(start_state) + ")";

    string state1 = "(" + remove_brackets_from_string(start_state) + to_string(this_state_number) + to_string(1) + ")";
    tm = add_state_to_tm(tm, head, head, temp_start_state, state1, ">");

    start_state = remove_brackets_from_string(start_state);

    string state2 = "(" + start_state + to_string(this_state_number) + to_string(2) + ")";
    tm = add_state_to_tm(tm, a, c, state1, state2, "<");

    string state3 = "(" + start_state + to_string(this_state_number) + to_string(3) + ")";
    tm = add_state_to_tm(tm, head, head, state2, state3, "<");

    for (string single_letter : transition_alphabet)
    {
        string state5 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(d) + temp_splitter + start_state + to_string(this_state_number) + to_string(5) + ")";
        string state4_letter = "(" + start_state + to_string(this_state_number) + to_string(4) + remove_brackets_from_string(single_letter) + ")";
        tm = add_state_to_tm(tm, single_letter, head, state3, state4_letter, ">");

        tm = add_state_to_tm(tm, head, single_letter, state4_letter, state5, ">");

        string state4_blank = "(" + start_state + to_string(this_state_number) + to_string(4) + BLANK + ")";
        tm = add_state_to_tm(tm, BLANK, head, state3, state4_blank, ">");

        tm = add_state_to_tm(tm, head, BLANK, state4_blank, state5, ">");

        tm = add_all_alphabet_and_splitter_states(tm, splitter, state5, state5, ">", transition_alphabet);

        string state6 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(d) + temp_splitter + start_state + to_string(this_state_number) + to_string(6) + ")";
        tm = add_state_to_tm(tm, head, head, state5, state6, ">");

        string state7 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(d) + temp_splitter + start_state + to_string(this_state_number) + to_string(7) + ")";
        tm = add_state_to_tm(tm, b, d, state6, state7, "<");

        string state8 = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(d) + temp_splitter + start_state + to_string(this_state_number) + to_string(8) + ")";
        tm = add_state_to_tm(tm, head, head, state7, state8, "<");

        tm = add_all_alphabet_and_splitter_states(tm, splitter, state8, state8, "<", transition_alphabet);

        string temp_end_state = "(" + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(d) + temp_splitter + remove_brackets_from_string(end_state) + ")";
        if (end_state == "(accept)" || end_state == "(reject)")
        {
            temp_end_state = end_state;
        }
        tm = add_state_to_tm(tm, head, head, state8, temp_end_state, "-");

        // tm = add_state_to_tm(tm, splitter, splitter, state4_letter, "(_" + temp_splitter + d + temp_splitter + start_state + to_string(this_state_number) + to_string(5) + ")", "<");
    }

    return tm;
}

TuringMachine add_no_left_states(TuringMachine tm, string splitter, string head, pair<string, vector<string>> input_state, tuple<string, vector<string>, string> output_state, vector<string> transition_alphabet)
{
    string a = input_state.second[0];
    string b = input_state.second[1];
    string c = get<1>(output_state)[0];
    string d = get<1>(output_state)[1];
    string start_state = input_state.first;
    string end_state = get<0>(output_state);

    if (start_state == INITIAL_STATE)
    {
        start_state = "(START)";
    }

    if (end_state == INITIAL_STATE)
    {
        end_state = "(START)";
    }

    int this_state_number = get_next_number();

    string temp_splitter = remove_brackets_from_string(splitter);
    string state_prefix = remove_brackets_from_string(a) + temp_splitter + remove_brackets_from_string(b) + temp_splitter;

    string temp_start_state = "(" + state_prefix + remove_brackets_from_string(start_state) + ")";

    string state1 = "(" + remove_brackets_from_string(start_state) + to_string(this_state_number) + to_string(1) + ")";
    tm = add_state_to_tm(tm, head, head, temp_start_state, state1, ">");

    start_state = remove_brackets_from_string(start_state);

    string state2 = "(" + start_state + to_string(this_state_number) + to_string(2) + ")";
    tm = add_state_to_tm(tm, a, c, state1, state2, ">");

    tm = add_all_alphabet_and_splitter_states(tm, splitter, state2, state2, ">", transition_alphabet);

    string state3 = "(" + start_state + to_string(this_state_number) + to_string(3) + ")";
    tm = add_state_to_tm(tm, head, head, state2, state3, ">");

    string state4 = "(" + start_state + to_string(this_state_number) + to_string(4) + ")";
    tm = add_state_to_tm(tm, b, d, state3, state4, "<");

    string state5 = "(" + start_state + to_string(this_state_number) + to_string(5) + ")";
    tm = add_state_to_tm(tm, head, head, state4, state5, "<");

    for (string single_letter : transition_alphabet)
    {
        string state7 = "(" + remove_brackets_from_string(c) + temp_splitter + remove_brackets_from_string(single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(7) + ")";

        string state6_letter = "(" + start_state + to_string(this_state_number) + to_string(6) + remove_brackets_from_string(single_letter) + ")";
        tm = add_state_to_tm(tm, single_letter, head, state5, state6_letter, ">");

        tm = add_state_to_tm(tm, head, single_letter, state6_letter, state7, "<");

        string state8 = "(" + remove_brackets_from_string(c) + temp_splitter + remove_brackets_from_string(single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(8) + ")";
        tm = add_state_to_tm(tm, head, head, state7, state8, "<");

        tm = add_all_alphabet_and_splitter_states(tm, splitter, state8, state8, "<", transition_alphabet);

        string temp_end_state = "(" + remove_brackets_from_string(c) + temp_splitter + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(end_state) + ")";
        if (end_state == "(accept)" || end_state == "(reject)")
        {
            temp_end_state = end_state;
        }
        tm = add_state_to_tm(tm, head, head, state8, temp_end_state, "-");

        // tm = add_state_to_tm(tm, splitter, splitter, state6_letter, "(" + c + temp_splitter + "_" + temp_splitter + start_state + to_string(this_state_number) + to_string(7) + ")", "<");
    }

    return tm;
}

TuringMachine add_no_right_states(TuringMachine tm, string splitter, string head, pair<string, vector<string>> input_state, tuple<string, vector<string>, string> output_state, vector<string> transition_alphabet)
{
    string a = input_state.second[0];
    string b = input_state.second[1];
    string c = get<1>(output_state)[0];
    string d = get<1>(output_state)[1];
    string start_state = input_state.first;
    string end_state = get<0>(output_state);

    if (start_state == INITIAL_STATE)
    {
        start_state = "(START)";
    }

    if (end_state == INITIAL_STATE)
    {
        end_state = "(START)";
    }

    int this_state_number = get_next_number();
    string temp_splitter = remove_brackets_from_string(splitter);
    string state_prefix = remove_brackets_from_string(a) + temp_splitter + remove_brackets_from_string(b) + temp_splitter;

    string temp_start_state = "(" + state_prefix + remove_brackets_from_string(start_state) + ")";

    string state1 = "(" + remove_brackets_from_string(start_state) + to_string(this_state_number) + to_string(1) + ")";
    tm = add_state_to_tm(tm, head, head, temp_start_state, state1, ">");

    start_state = remove_brackets_from_string(start_state);

    string state2 = "(" + start_state + to_string(this_state_number) + to_string(2) + ")";
    tm = add_state_to_tm(tm, a, c, state1, state2, ">");

    tm = add_all_alphabet_and_splitter_states(tm, splitter, state2, state2, ">", transition_alphabet);

    string state3 = "(" + start_state + to_string(this_state_number) + to_string(3) + ")";
    tm = add_state_to_tm(tm, head, d, state2, state3, ">");

    string state4 = "(" + start_state + to_string(this_state_number) + to_string(4) + ")";
    tm = add_state_to_tm(tm, b, head, state3, state4, ">");

    for (string single_letter : transition_alphabet)
    {
        string state5 = "(" + remove_brackets_from_string(c) + temp_splitter + remove_brackets_from_string(single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(5) + ")";
        tm = add_state_to_tm(tm, single_letter, single_letter, state4, state5, "<");
        string state6 = "(" + remove_brackets_from_string(c) + temp_splitter + remove_brackets_from_string(single_letter) + temp_splitter + start_state + to_string(this_state_number) + to_string(6) + ")";

        tm = add_all_alphabet_and_splitter_states(tm, splitter, state6, state6, "<", transition_alphabet);

        tm = add_state_to_tm(tm, head, head, state5, state6, "<");

        string temp_end_state = "(" + remove_brackets_from_string(c) + temp_splitter + remove_brackets_from_string(single_letter) + temp_splitter + remove_brackets_from_string(end_state) + ")";
        if (end_state == "(accept)" || end_state == "(reject)")
        {
            temp_end_state = end_state;
        }
        tm = add_state_to_tm(tm, head, head, state6, temp_end_state, "-");
    }
    string state4_5 = "(" + remove_brackets_from_string(c) + temp_splitter + "_" + temp_splitter + start_state + to_string(this_state_number) + to_string(45) + ")";
    tm = add_state_to_tm(tm, splitter, BLANK, state4, state4_5, ">");
    string state4_75 = "(" + remove_brackets_from_string(c) + temp_splitter + "_" + temp_splitter + start_state + to_string(this_state_number) + to_string(475) + ")";
    tm = add_state_to_tm(tm, BLANK, splitter, state4_5, state4_75, "<");
    tm = add_state_to_tm(tm, BLANK, BLANK, state4_75, "(" + remove_brackets_from_string(c) + temp_splitter + "_" + temp_splitter + start_state + to_string(this_state_number) + to_string(5) + ")", "<");

    return tm;
}

TuringMachine add_no_no_states(TuringMachine tm, string splitter, string head, pair<string, vector<string>> input_state, tuple<string, vector<string>, string> output_state, vector<string> transition_alphabet)
{
    string a = input_state.second[0];
    string b = input_state.second[1];
    string c = get<1>(output_state)[0];
    string d = get<1>(output_state)[1];
    string start_state = input_state.first;
    string end_state = get<0>(output_state);

    if (start_state == INITIAL_STATE)
    {
        start_state = "(START)";
    }

    if (end_state == INITIAL_STATE)
    {
        end_state = "(START)";
    }

    int this_state_number = get_next_number();

    string temp_splitter = remove_brackets_from_string(splitter);
    string state_prefix = remove_brackets_from_string(a) + temp_splitter + remove_brackets_from_string(b) + temp_splitter;

    string temp_start_state = "(" + state_prefix + remove_brackets_from_string(start_state) + ")";

    string state1 = "(" + remove_brackets_from_string(start_state) + to_string(this_state_number) + to_string(1) + ")";
    tm = add_state_to_tm(tm, head, head, temp_start_state, state1, ">");

    start_state = remove_brackets_from_string(start_state);

    string state2 = "(" + start_state + to_string(this_state_number) + to_string(2) + ")";
    tm = add_state_to_tm(tm, a, c, state1, state2, ">");

    tm = add_all_alphabet_and_splitter_states(tm, splitter, state2, state2, ">", transition_alphabet);

    string state3 = "(" + start_state + to_string(this_state_number) + to_string(3) + ")";
    tm = add_state_to_tm(tm, head, head, state2, state3, ">");

    string state4 = "(" + start_state + to_string(this_state_number) + to_string(4) + ")";
    tm = add_state_to_tm(tm, b, d, state3, state4, "<");

    string state5 = "(" + start_state + to_string(this_state_number) + to_string(5) + ")";
    tm = add_state_to_tm(tm, head, head, state4, state5, "<");

    tm = add_all_alphabet_and_splitter_states(tm, splitter, state5, state5, "<", transition_alphabet);

    string temp_end_state = "(" + remove_brackets_from_string(c) + temp_splitter + remove_brackets_from_string(d) + temp_splitter + remove_brackets_from_string(end_state) + ")";
    if (end_state == "(accept)" || end_state == "(reject)")
    {
        temp_end_state = end_state;
    }

    tm = add_state_to_tm(tm, head, head, state5, temp_end_state, "-");

    return tm;
}

set<string> collect_identifiers(TuringMachine tm)
{
    set<string> res;
    transitions_t transitions = tm.transitions;
    for (auto transition : transitions)
    {
        pair<string, vector<string>> input_state = transition.first;
        tuple<string, vector<string>, string> output_state = transition.second;

        string a = input_state.second[0];
        string b = input_state.second[1];
        string c = get<1>(output_state)[0];
        string d = get<1>(output_state)[1];

        res.insert(a);
        res.insert(b);
        res.insert(c);
        res.insert(d);
    }
    for (string letter : tm.input_alphabet)
    {
        res.insert(letter);
    }
    res.insert("_");
    return res;
}

TuringMachine add_states(TuringMachine input_tm, string splitter, string head, TuringMachine tm, vector<string> transition_alphabet)
{
    transitions_t transitions = input_tm.transitions;
    for (auto transition : transitions)
    {
        pair<string, vector<string>> input_state = transition.first;
        tuple<string, vector<string>, string> output_state = transition.second;
        string permutation = get<2>(output_state);

        if (permutation == "--")
        {
            tm = add_no_no_states(tm, splitter, head, input_state, output_state, transition_alphabet);
        }
        else if (permutation == "->")
        {
            tm = add_no_right_states(tm, splitter, head, input_state, output_state, transition_alphabet);
        }
        else if (permutation == "-<")
        {
            tm = add_no_left_states(tm, splitter, head, input_state, output_state, transition_alphabet);
        }
        else if (permutation == ">-")
        {
            tm = add_right_no_states(tm, splitter, head, input_state, output_state, transition_alphabet);
        }
        else if (permutation == "<-")
        {
            tm = add_left_no_states(tm, splitter, head, input_state, output_state, transition_alphabet);
        }
        else if (permutation == ">>")
        {
            tm = add_right_right_states(tm, splitter, head, input_state, output_state, transition_alphabet);
        }
        else if (permutation == "<<")
        {
            tm = add_left_left_states(tm, splitter, head, input_state, output_state, transition_alphabet);
        }
        else if (permutation == "><")
        {
            tm = add_right_left_states(tm, splitter, head, input_state, output_state, transition_alphabet);
        }
        else if (permutation == "<>")
        {
            tm = add_left_right_states(tm, splitter, head, input_state, output_state, transition_alphabet);
        }
    }

    return tm;
}

// TODO add splitter and head to final alphabet

int main(int argc, char *argv[])
{
    string filename, output_filename;
    string input;
    int ok = 0;
    for (int i = 1; i < argc; i++)
    {
        string arg = argv[i];
        if (ok == 0)
            filename = arg;
        else if (ok == 1)
            output_filename = arg;
        else
            cerr << ("Too many arguments");
        ++ok;
    }
    if (ok != 2)
        cerr << ("Not enough arguments");

    FILE *f = fopen(filename.c_str(), "r");
    if (!f)
    {
        cerr << "ERROR: File " << filename << " does not exist\n";
        return 1;
    }
    TuringMachine input_tm = read_tm_from_file(f);

    if (input_tm.num_tapes != 2)
    {
        cerr << "ERROR: Input tape should have 2 tapes\n";
        return 1;
    }

    string head = create_head(input_tm.input_alphabet);
    string splitter = create_splitter(input_tm.input_alphabet);

    set<string> identifiers = collect_identifiers(input_tm);
    vector<string> transition_alphabet(identifiers.begin(), identifiers.end());

    TuringMachine output_tm = create_empty_from_input(input_tm);

    output_tm = prepare_single_tape_machine(output_tm, splitter, head, transition_alphabet);

    output_tm = add_states(input_tm, splitter, head, output_tm, transition_alphabet);

    ofstream out_file;
    out_file.open(output_filename);
    output_tm.save_to_file(out_file);
    out_file.close();
}
