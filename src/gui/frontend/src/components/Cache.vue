<template>
  <div>
    <div v-for="(value, key) in cache" :key="key">
      <div class="c_key">{{ key }}</div>
      <div class="c_value">{{ value }}</div>
    </div>
  </div>
</template>

<script>
export default {
  name: "cacheComponent",
  props: {
    cache: Object,
  },
};
</script>

<style>
.c_key {
  margin: 1px 1px 0px 0px;
  border-style: solid;
  display: inline-block;
  width: 70px;
}
.c_value {
  border-style: solid;
  display: inline-block;
  width: 300px;
}
</style>