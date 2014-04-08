#ifndef ASSESSMENT_H
#define ASSESSMENT_H

class Question;

class Assessment
{
public:
    QList<Question> questionList;
    bool isDone;
};

#endif // ASSESSMENT_H
