#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "permutation.h"

typedef enum {
  LEFT,
  RIGHT,
} Direction;

typedef struct Node_ {
  int key;
  struct Node_* children[2];
} Node;

// 出力関数。講義のハンドアウトより引用
void print(Node* current, int depth) {
  // printf("print\n");
  if (current == NULL) {
    return;
  }
  // right
  print(current->children[RIGHT], depth + 1);
  // current
  printf("|");
  for (int i = 0; i < depth; i++) {
    printf("  ");
  }
  printf("%d\n", current->key);
  // left
  print(current->children[LEFT], depth + 1);
}

// 木の作成
bool insert(Node** p, int key) {
  Node* current = *p;
  if (current == NULL) {
    // NULLの場合新しいノードを作成して挿入
    Node* new = (Node*)malloc(sizeof(Node));
    new->key = key;
    new->children[LEFT] = NULL;
    new->children[RIGHT] = NULL;
    *p = new;
    return true;
  }
  // 既存のノードの場合、再帰的に探索
  if (current->key < key) {
    // 右の子に挿入する際、左の子が存在する場合は中断する
    if (current->children[LEFT] != NULL) return false;
    return insert(&current->children[RIGHT], key);
  } else if (key < current->key) {
    return insert(&current->children[LEFT], key);
  } else {
    assert(false);
  }
  return true;
}

typedef struct treeReturn {
  int depth;
  int weight;
} TreeReturn;

// 木の構造をチェックする再帰関数
TreeReturn treeCheckerInner(Node* tree, int prevDepth, int* treeType) {
  prevDepth++;
  // 葉の場合、木の深さと重みを返す
  if (tree->children[LEFT] == NULL && tree->children[RIGHT] == NULL) {
    TreeReturn treeReturn = {prevDepth, 1};
    return treeReturn;
  }
  TreeReturn leftReturn = {prevDepth, 0}, rightReturn = {prevDepth, 0};
  // 構造を再帰的に探索
  if (tree->children[LEFT] != NULL) {
    leftReturn = treeCheckerInner(tree->children[LEFT], prevDepth, treeType);
  }
  if (tree->children[RIGHT] != NULL) {
    rightReturn = treeCheckerInner(tree->children[RIGHT], prevDepth, treeType);
  }
  // 重みの差が1以上の場合、木の構造は完全二分木ではない
  if (abs(leftReturn.weight - rightReturn.weight) > 1) {
    *treeType = 1;
  }
  // 深さの差が1以上の場合、木の構造はAVL木ではない
  if (abs(leftReturn.depth - rightReturn.depth) > 1) {
    *treeType = 0;
  }
  // 深さと重みを返す
  TreeReturn res = {leftReturn.depth > rightReturn.depth ? leftReturn.depth
                                                         : rightReturn.depth,
                    leftReturn.weight + rightReturn.weight + 1};
  return res;
};

typedef struct treeCheckerReturn {
  int height;
  int treeType;
} TreeCheckerReturn;

// 木の構造をチェックする関数
TreeCheckerReturn treeChecker(Node* tree, int n) {
  int treeType = 2;
  TreeReturn res = treeCheckerInner(tree, -1, &treeType);
  TreeCheckerReturn result = {res.depth, treeType};
  return result;
}

int main() {
  int n;
  printf("input data num: ");
  scanf("%d", &n);
  printf("\n\n");

  // 入力された長さの、全ての順列を作成
  int** p = createPermutation(n);

  // 木の個数、高さ、構造を記録する配列
  // 添え字0: 通常の二分木、添え字1: AVL木、添え字2: 完全二分木、添え字3: 全体
  int counter[4] = {0};
  TreeCheckerReturn* results =
      (TreeCheckerReturn*)malloc(sizeof(TreeCheckerReturn) * factorial(n));

  // 全ての順列に対して木を作成し、構造をチェック
  for (int i = 0; i < factorial(n); i++) {
    Node* tree = NULL;
    bool isInvalidInsert = false;
    for (int j = 0; j < n; j++) {
      if (!insert(&tree, p[i][j])) {
        isInvalidInsert = true;
        break;
      }
    }
    if (isInvalidInsert) continue;
    print(tree, 0);
    TreeCheckerReturn res = treeChecker(tree, n);
    char typeStr[10];
    switch (res.treeType) {
      case 0:
        strcpy(typeStr, "normal");
        break;
      case 1:
        strcpy(typeStr, "AVL");
        break;
      case 2:
        strcpy(typeStr, "complete");
        break;
    }
    printf("\ntype: %s", typeStr);
    results[counter[3]] = res;
    counter[3]++;
    counter[res.treeType]++;
    printf("\n------------\n\n");
  }

  // 完全二分木はAVL木でもあるため、AVL木の数に完全二分木の数を加算
  counter[1] += counter[2];

  // 高さの平均と分散を計算
  double heightAverage[4] = {0};
  double heightVar[4] = {0};
  for (int i = 0; i < counter[3]; i++) {
    heightAverage[results[i].treeType] += results[i].height;
    heightAverage[3] += results[i].height;
    if (results[i].treeType == 2) {
      heightAverage[1] += results[i].height;
    }
  }
  heightAverage[0] /= counter[0];
  heightAverage[1] /= counter[1];
  heightAverage[2] /= counter[2];
  heightAverage[3] /= counter[3];

  for (int i = 0; i < counter[3]; i++) {
    heightVar[results[i].treeType] +=
        pow(results[i].height - heightAverage[results[i].treeType], 2);
    heightVar[3] += pow(results[i].height - heightAverage[3], 2);
    if (results[i].treeType == 2) {
      heightVar[1] += pow(results[i].height - heightAverage[1], 2);
    }
  }
  heightVar[0] /= counter[0];
  heightVar[1] /= counter[1];
  heightVar[2] /= counter[2];
  heightVar[3] /= counter[3];

  printf("tree num %d\n", counter[3]);
  printf("[Total] num: %d, ave: %f, var: %f\n", counter[3], heightAverage[3],
         heightVar[3]);
  printf("[Normal] num: %d, ave: %f, var: %f\n", counter[0], heightAverage[0],
         heightVar[0]);
  printf("[AVL] num: %d, ave: %f, var: %f\n", counter[1], heightAverage[1],
         heightVar[1]);
  printf("[Complete] num: %d, ave: %f, var: %f\n", counter[2], heightAverage[2],
         heightVar[2]);
}